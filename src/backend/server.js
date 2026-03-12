import express from "express";
import dotenv from "dotenv";
import { MongoClient } from "mongodb";
import cors from "cors";
import { WebSocketServer } from "ws";
import { createServer } from "http";

dotenv.config();

const app = express();
app.use(express.json());
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

app.post("/files", async (req,res) => {
  try{
    const file = req.body;
    const result = await db.collection("files").insertOne(file);
    res.json({success: true, id: result.insertedId});
  } catch (err) {
    res.status(500).json({error: err.message});
  }
});

app.get("/files", async (req,res) => {
  try {
    const files = await db.collection("files").find({ channel: req.params.channel }).toArray();
    res.json(files);
  } catch (err) {
    res.status(500).json({error: err.message });
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


const channels = new Map();

function getChannel(name) {
  if (!channels.has(name)) channels.set(name, new Set());
  return channels.get(name);
}

function broadcast(channelName, payload, exclude = null) {
  const members = channels.get(channelName);
  if (!members) return;

  const data = JSON.stringify(payload);

  for (const client of members) {
    if (client !== exclude && client.readyState === 1) {
      client.send(data);
    }
  }
}


const server = createServer(app);
const wss = new WebSocketServer({ server });


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

wss.on("connection", (ws) => {
  ws.isAlive = true;
  ws.currentChannel = null;
  ws.displayName = "anonymous";

  ws.on("pong", () => {
    ws.isAlive = true;
  });

  ws.on("message", (raw) => {
    let msg;

    try {
      msg = JSON.parse(raw);
    } catch {
      return;
    }

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
    }

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


connectDB().then(() => {
  server.listen(PORT, () => {
    console.log(`Server running on port ${PORT}`);
  });
});
