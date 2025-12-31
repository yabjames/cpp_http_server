import cluster from "node:cluster";
import os from "node:os";
import express from "express";
import http from "http";

if (cluster.isPrimary) {
  const numCPUs = os.cpus().length;
  for (let i = 0; i < numCPUs; i++) {
    cluster.fork();
  }
} else {
  const app = express();

  app.post("/test2/:id/foo/:user", (req, res) => {
    res.json({
      id: req.params.id,
      user: req.params.user,
    });
  });

  // disable TLS
  http.createServer(app).listen(3490, () => {
    console.log("Express server listening on http://localhost:3490");
  });
}
