import express from "express";
import dotenv from "dotenv";
import { MongoClient } from "mongodb";
import cors from "cors";
import { WebSocketServer } from "ws";
import { createServer } from "http";

dotenv.config();

const app = express();
app.use(express.json({ limit: "15mb" })); // allow larger file uploads via REST too
app.use(cors());

const PORT = process.env.PORT || 3000;
const uri = process.env.MONGODB_URI;

if (!uri) {
  console.error("MONGODB_URI not set");
  process.exit(1);
}

let db;

async function connectDB() {
  try {
    const client = new MongoClient(uri);
    await client.connect();
    db = client.db();
    console.log("MongoDB connected");
  } catch (err) {
    console.error(err);
    process.exit(1);
  }
}

// ── REST routes ──────────────────────────────────────────────────────────────

app.get("/", (req, res) => {
  res.json({ status: "NoteFlow API running" });
});

app.post("/notes", async (req, res) => {
  try {
    const note = req.body;
    const result = await db.collection("notes").insertOne(note);
    res.json({ success: true, id: result.insertedId });
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

app.get("/notes", async (req, res) => {
  try {
    const notes = await db.collection("notes").find().toArray();
    res.json(notes);
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

app.post("/files", async (req, res) => {
  try {
    const file = req.body;
    const result = await db.collection("files").insertOne(file);
    res.json({ success: true, id: result.insertedId });
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

app.get("/files/:channel", async (req, res) => {
  try {
    const files = await db
      .collection("files")
      .find({ channel: req.params.channel })
      .toArray();
    res.json(files);
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

// ── WebSocket setup ──────────────────────────────────────────────────────────

const channels = new Map();

function getChannel(name) {
  if (!channels.has(name)) channels.set(name, new Set());
  return channels.get(name);
}

function broadcast(channelName, payload, exclude = null) {
  const members = channels.get(channelName);
  if (!members) return;

  const data = typeof payload === "string" ? payload : JSON.stringify(payload);

  for (const client of members) {
    if (client !== exclude && client.readyState === 1) {
      client.send(data);
    }
  }
}

const server = createServer(app);

// 10 MB max payload — large enough for most source files base64-encoded
const wss = new WebSocketServer({ server, maxPayload: 10 * 1024 * 1024 });

// ── Keep-alive ping/pong ─────────────────────────────────────────────────────

const keepAlive = setInterval(() => {
  wss.clients.forEach((ws) => {
    if (!ws.isAlive) {
      ws.terminate();
      return;
    }
    ws.isAlive = false;
    ws.ping();
  });
}, 25000);

wss.on("close", () => clearInterval(keepAlive));

// ── Connection handler ───────────────────────────────────────────────────────

wss.on("connection", (ws) => {
  ws.isAlive = true;
  ws.currentChannel = null;
  ws.displayName = "anonymous";

  ws.on("pong", () => {
    ws.isAlive = true;
  });

  ws.on("message", async (raw) => {
    let msg;

    try {
      msg = JSON.parse(raw);
    } catch {
      return;
    }

    // ── JOIN ──────────────────────────────────────────────────────────────
    if (msg.type === "join") {
      // leave previous channel
      if (ws.currentChannel) {
        const prev = channels.get(ws.currentChannel);
        if (prev) {
          prev.delete(ws);
          broadcast(ws.currentChannel, {
            type: "presence",
            channel: ws.currentChannel,
            count: prev.size,
          });
        }
      }

      ws.currentChannel = msg.channel;
      ws.displayName = msg.sender || "anonymous";

      getChannel(msg.channel).add(ws);

      broadcast(msg.channel, {
        type: "presence",
        channel: msg.channel,
        count: channels.get(msg.channel).size,
      });

      // ── Send existing files in this channel to the newly joined user ──
      try {
        const existingFiles = await db
          .collection("files")
          .find({ channel: msg.channel })
          .toArray();

        for (const file of existingFiles) {
          ws.send(
            JSON.stringify({
              type: "file_upload",
              channel: file.channel,
              sender: file.sender,
              filename: file.filename,
              content: file.content, // already base64
            })
          );
        }
      } catch (err) {
        console.error("Failed to send existing files on join:", err.message);
      }
    }

    // ── CHAT MESSAGE ─────────────────────────────────────────────────────
    else if (msg.type === "message") {
      if (!msg.channel || !msg.text) return;

      broadcast(
        msg.channel,
        {
          type: "message",
          channel: msg.channel,
          sender: msg.sender,
          text: msg.text,
          ts: msg.ts || new Date().toTimeString().slice(0, 5),
        },
        ws
      );
    }

    // ── FILE EDIT (real-time collaborative editing) ───────────────────────
    else if (msg.type === "file_edit") {
      if (!msg.channel || !msg.filename) return;

      broadcast(
        msg.channel,
        {
          type: "file_edit",
          channel: msg.channel,
          sender: msg.sender,
          filename: msg.filename,
          position: msg.position,
          length: msg.length,
          text: msg.text,
          isAddition: msg.isAddition,
        },
        ws // exclude the sender
      );
    }

    // ── FILE UPLOAD (new file shared to channel) ─────────────────────────
    else if (msg.type === "file_upload") {
      if (!msg.channel || !msg.filename || !msg.content) return;

      // Persist to MongoDB so late joiners get it
      try {
        await db.collection("files").updateOne(
          { channel: msg.channel, filename: msg.filename },
          {
            $set: {
              channel: msg.channel,
              sender: msg.sender,
              filename: msg.filename,
              content: msg.content, // base64 string
              updatedAt: new Date(),
            },
          },
          { upsert: true } // insert if new, update if same filename re-uploaded
        );
      } catch (err) {
        console.error("Failed to persist file upload:", err.message);
      }

      // Relay to everyone else in the channel
      broadcast(
        msg.channel,
        {
          type: "file_upload",
          channel: msg.channel,
          sender: msg.sender,
          filename: msg.filename,
          content: msg.content,
        },
        ws
      );
    }
  });

  ws.on("close", () => {
    if (ws.currentChannel) {
      const ch = channels.get(ws.currentChannel);
      if (ch) {
        ch.delete(ws);
        broadcast(ws.currentChannel, {
          type: "presence",
          channel: ws.currentChannel,
          count: ch.size,
        });
      }
    }
  });
});

// ── Start ────────────────────────────────────────────────────────────────────

connectDB().then(() => {
  server.listen(PORT, () => {
    console.log(`Server running on port ${PORT}`);
  });
});
