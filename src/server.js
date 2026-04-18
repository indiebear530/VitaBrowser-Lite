const express = require("express");
const fetch = require("node-fetch");
const path = require("path");

const app = express();
const PORT = 3000;

// Serve static files (index.html)
app.use(express.static(__dirname));

// Homepage
app.get("/", (req, res) => {
  res.sendFile(path.join(__dirname, "index.html"));
});

// SEARCH ROUTE
app.get("/search", (req, res) => {
  let query = req.query.q || "";

  // If user types a full URL, go directly
  if (query.startsWith("http://") || query.startsWith("https://")) {
    return res.redirect("/proxy?url=" + encodeURIComponent(query));
  }

  // Otherwise search DuckDuckGo Lite
  let searchUrl = "https://duckduckgo.com/html/?q=" + encodeURIComponent(query);
  res.redirect("/proxy?url=" + encodeURIComponent(searchUrl));
});

// PROXY ROUTE
app.get("/proxy", async (req, res) => {
  try {
    let url = req.query.url;

    if (!url) return res.send("No URL provided.");

    const response = await fetch(url);
    let html = await response.text();

    // Remove scripts (important for Vita)
    html = html.replace(/<script[\s\S]*?>[\s\S]*?<\/script>/gi, "");

    // Rewrite absolute links
    html = html.replace(/href="(https?:\/\/[^"]+)"/g, (match, link) => {
      return `href="/proxy?url=${encodeURIComponent(link)}"`;
    });

    // Rewrite relative links
    html = html.replace(/href="\/([^"]*)"/g, (match, link) => {
      const base = new URL(url).origin;
      return `href="/proxy?url=${encodeURIComponent(base + "/" + link)}"`;
    });

    // Rewrite form actions (VERY important for search pages)
    html = html.replace(/action="(https?:\/\/[^"]+)"/g, (match, link) => {
      return `action="/proxy?url=${encodeURIComponent(link)}"`;
    });

    res.send(html);

  } catch (e) {
    res.send("Error loading site.");
  }
});

// Start server
app.listen(PORT, () => {
  console.log("Vita Proxy running on port " + PORT);
});
