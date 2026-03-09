import express from "express";
import dotenv from "dotenv";
import { MongoClient } from "mongodb";
import cors from "cors";

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
    db = client.db(); // database name comes from URI

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

    res.json({
      success: true,
      id: result.insertedId
    });

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

connectDB().then(() => {
  app.listen(PORT, () => {
    console.log(`Server running on port ${PORT}`);
  });
});