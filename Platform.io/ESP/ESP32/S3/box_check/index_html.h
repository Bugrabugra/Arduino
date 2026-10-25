#pragma once
// Web UI served by the ESP32 at "/".
// QR decoding uses jsQR in the browser (the computer needs internet: libraries load from a CDN).

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Box check</title>
<link href="https://fonts.googleapis.com/css2?family=Barlow+Condensed:wght@500;700&family=Barlow:wght@400;600&display=swap" rel="stylesheet">
<script src="https://cdn.jsdelivr.net/npm/jsqr@1.4.0/dist/jsQR.js"></script>
<script src="https://cdnjs.cloudflare.com/ajax/libs/qrcodejs/1.0.0/qrcode.min.js"></script>
<style>
:root{
  --bg:#e6eaee; --surface:#ffffff; --ink:#1c2833; --muted:#5d6b78;
  --cardboard:#b8875a; --ok:#1f9d55; --ok-light:#dff3e7; --error:#d64545; --pending:#e0a526;
  --line:#c9d1d8;
}
@media (prefers-color-scheme: dark){
  :root{ --bg:#161c22; --surface:#20272f; --ink:#e8edf1; --muted:#94a2ae;
         --ok-light:#16382a; --line:#36414c; color-scheme:dark; }
}
*{box-sizing:border-box}
html,body{margin:0;height:100%}
body{background:var(--bg);color:var(--ink);font:16px/1.4 Barlow,system-ui,sans-serif}
.stage{display:grid;grid-template-columns:minmax(280px,38%) 1fr;gap:20px;padding:20px;height:100%}
@media (max-width:820px){.stage{grid-template-columns:1fr;height:auto}}
.camera{display:flex;flex-direction:column;gap:12px}
.frame{position:relative;background:#000;border-radius:6px;overflow:hidden;aspect-ratio:4/3;border:3px solid var(--line)}
.frame canvas{width:100%;height:100%;object-fit:contain;display:block}
.frame.detected{border-color:var(--ok)}
.status{font-size:14px;color:var(--muted);display:flex;justify-content:space-between;gap:8px}
.status b{color:var(--ink)}
.history{background:var(--surface);border-radius:6px;padding:12px 14px;font-size:14px;flex:1;overflow:auto;min-height:120px}
.history h3{margin:0 0 6px;font:600 15px Barlow}
.history ol{margin:0;padding-left:18px;color:var(--muted)}
.history li.good{color:var(--ok)} .history li.bad{color:var(--error)}
.box{display:flex;flex-direction:column;min-height:0}
.header{display:flex;align-items:flex-end;justify-content:space-between;gap:16px;border-bottom:6px solid var(--cardboard);padding-bottom:10px}
.box-name{font:600 18px Barlow;color:var(--muted)}
.counter{font:700 clamp(64px,10vw,120px)/0.9 "Barlow Condensed",sans-serif;letter-spacing:-1px}
.counter small{font-size:.4em;color:var(--muted);font-weight:500;margin-left:6px}
.buttons{display:flex;gap:8px;flex-wrap:wrap}
button{font:600 15px Barlow;padding:10px 14px;border-radius:6px;border:2px solid var(--ink);background:transparent;color:var(--ink);cursor:pointer}
button.primary{background:var(--ink);color:var(--bg)}
button:focus-visible,textarea:focus-visible,select:focus-visible{outline:3px solid var(--pending);outline-offset:2px}
.part-list{list-style:none;margin:14px 0 0;padding:0;display:flex;flex-direction:column;gap:8px;overflow:auto}
.part{display:grid;grid-template-columns:1fr auto;align-items:center;background:var(--surface);border-left:10px solid var(--line);border-radius:4px;padding:12px 16px;transition:background .3s}
.part .name{font:700 26px "Barlow Condensed",sans-serif}
.part .code{font-size:13px;color:var(--muted)}
.part .qty{font:700 34px "Barlow Condensed",sans-serif}
.part.partial{border-left-color:var(--pending)}
.part.done{border-left-color:var(--ok);background:var(--ok-light)}
.part.done .qty::after{content:" \2713";color:var(--ok)}
.part.just-read{animation:flash .6s}
@keyframes flash{from{background:var(--ok);color:#fff}}
.alert{position:fixed;inset:0;display:none;place-items:center;text-align:center;color:#fff;z-index:5;padding:20px}
.alert.open{display:grid}
.alert.error{background:color-mix(in srgb,var(--error) 94%,transparent)}
.alert.complete{background:color-mix(in srgb,var(--ok) 94%,transparent)}
.alert h1{font:700 clamp(60px,12vw,150px)/1 "Barlow Condensed",sans-serif;margin:0}
.alert p{font-size:22px;margin:10px 0 20px}
.alert button{border-color:#fff;color:#fff;font-size:18px}
dialog{border:0;border-radius:8px;padding:20px;max-width:720px;width:92%;background:var(--surface);color:var(--ink)}
dialog textarea{width:100%;height:220px;font:15px/1.5 ui-monospace,Consolas,monospace;background:var(--bg);color:var(--ink);border:1px solid var(--line);border-radius:4px;padding:10px}
dialog .footer{display:flex;justify-content:flex-end;gap:8px;margin-top:12px}
dialog .hint{color:var(--muted);margin-top:0}
.labels{display:grid;grid-template-columns:repeat(auto-fill,minmax(170px,1fr));gap:12px;margin-top:12px}
.label{border:1px dashed var(--muted);padding:10px;text-align:center;font-size:12px;background:#fff;color:#000}
.label div{display:flex;justify-content:center;margin-bottom:6px}
@media print{ body>*:not(#labelDialog){display:none} #labelDialog{position:static;max-width:none;width:auto} #labelDialog .footer{display:none} }
@media (prefers-reduced-motion:reduce){*{animation:none!important;transition:none!important}}
</style>
</head>
<body>
<main class="stage">
  <section class="camera">
    <div class="frame" id="frame"><canvas id="canvas"></canvas></div>
    <div class="status"><span id="connection"></span><span><b id="fps">0</b> <span id="fpsLabel"></span></span></div>
    <div class="status">
      <label><span id="resolutionLabel"></span>
        <select id="resolution">
          <option value="vga">640x480</option>
          <option value="svga" selected>800x600</option>
          <option value="xga">1024x768</option>
        </select>
      </label>
    </div>
    <div class="history"><h3 id="historyTitle"></h3><ol id="historyList" reversed></ol></div>
  </section>

  <section class="box">
    <div class="header">
      <div>
        <div class="box-name" id="boxName"></div>
        <div class="counter" id="counter"></div>
      </div>
      <div class="buttons">
        <button class="primary" id="newBoxButton"></button>
        <button id="editButton"></button>
        <button id="labelsButton"></button>
      </div>
    </div>
    <ul class="part-list" id="partList"></ul>
  </section>
</main>

<div class="alert" id="alert"><div><h1 id="alertTitle"></h1><p id="alertText"></p><button id="alertButton"></button></div></div>

<dialog id="editDialog">
  <h2 id="editTitle" style="margin-top:0"></h2>
  <p class="hint" id="editHint"></p>
  <textarea id="listText" spellcheck="false"></textarea>
  <div class="footer"><button id="cancelButton"></button><button class="primary" id="saveButton"></button></div>
</dialog>

<dialog id="labelDialog">
  <h2 id="labelsTitle" style="margin-top:0"></h2>
  <p class="hint" id="labelsHint"></p>
  <div class="labels" id="labels"></div>
  <div class="footer"><button id="closeLabelsButton"></button><button class="primary" id="printButton"></button></div>
</dialog>

<script>
// All on-screen text lives here, so the UI language can be changed in one place.
const TEXT = {
  connecting: "Connecting to camera...",
  connected: "Camera connected",
  cameraDown: "Camera not responding, retrying...",
  qrLibMissing: "QR library failed to load: check the computer's internet connection",
  fps: "frames/s",
  resolution: "Resolution ",
  history: "Recent reads",
  parts: "parts",
  code: "Code",
  newBox: "Start new box",
  editList: "Edit list",
  testLabels: "Test labels",
  editTitle: "Box list",
  editHint: "One line per part: part code ; part name ; quantity. A QR counts for a part when its content starts with that code. Optional first line: #Box name",
  cancel: "Cancel",
  save: "Save and start new box",
  labelsTitle: "Test labels",
  labelsHint: "Print them or show them to the camera from a phone. The last two are one extra part and one unknown part, to test the warnings.",
  close: "Close",
  print: "Print",
  stop: "STOP",
  notInBox: "This part is not in this box: ",
  extraPart: "Extra part: {name} is already complete.",
  unknownLog: "Not in list: ",
  extraLog: "Extra: ",
  extraSuffix: " (extra)",
  unknownLabel: "Unknown part",
  boxComplete: "BOX COMPLETE",
  boxCompleteText: "Close the box and take it off the table.",
  continue: "Continue",
  defaultBoxName: "Box"
};

const DEFAULT_LIST = `#Coffee Corner - Box 1
10001;Side panel;2
10002;Top panel;1
10003;Bottom panel;1
10004;Shelf;3
10005;Back panel;1`;

const STORAGE_KEY = "boxList";
const SERIAL_DIGITS = 10;

let boxName = TEXT.defaultBoxName;
let parts = [];            // [{ code, name, qty, count }]
let readCodes = new Set(); // full QR contents already processed in this box
let alertOpen = false;

const $ = id => document.getElementById(id);

function applyStaticText() {
  $("connection").textContent = TEXT.connecting;
  $("fpsLabel").textContent = TEXT.fps;
  $("resolutionLabel").textContent = TEXT.resolution;
  $("historyTitle").textContent = TEXT.history;
  $("newBoxButton").textContent = TEXT.newBox;
  $("editButton").textContent = TEXT.editList;
  $("labelsButton").textContent = TEXT.testLabels;
  $("editTitle").textContent = TEXT.editTitle;
  $("editHint").textContent = TEXT.editHint;
  $("cancelButton").textContent = TEXT.cancel;
  $("saveButton").textContent = TEXT.save;
  $("labelsTitle").textContent = TEXT.labelsTitle;
  $("labelsHint").textContent = TEXT.labelsHint;
  $("closeLabelsButton").textContent = TEXT.close;
  $("printButton").textContent = TEXT.print;
}

// ---- part list parsing and storage ----
function parseList(text) {
  boxName = TEXT.defaultBoxName;
  const result = [];
  text.split(/\r?\n/).forEach(rawLine => {
    const line = rawLine.trim();
    if (!line) return;
    if (line.startsWith("#")) { boxName = line.slice(1).trim(); return; }
    const [code, name, qty] = line.split(";").map(field => (field || "").trim());
    if (code) result.push({ code, name: name || code, qty: Math.max(1, parseInt(qty) || 1), count: 0 });
  });
  parts = result;
}

function serializeList() {
  return "#" + boxName + "\n" + parts.map(p => `${p.code};${p.name};${p.qty}`).join("\n");
}

function saveList() {
  try { localStorage.setItem(STORAGE_KEY, serializeList()); } catch (e) {}
}

function loadList() {
  let stored = null;
  try { stored = localStorage.getItem(STORAGE_KEY); } catch (e) {}
  parseList(stored || DEFAULT_LIST);
}

// ---- rendering ----
// Returns true when every part in the box is complete.
function render(highlightCode) {
  const total = parts.reduce((sum, p) => sum + p.qty, 0);
  const done = parts.reduce((sum, p) => sum + Math.min(p.count, p.qty), 0);
  $("boxName").textContent = boxName;
  $("counter").innerHTML = `${done}<small>/ ${total} ${TEXT.parts}</small>`;

  const list = $("partList");
  list.innerHTML = "";
  parts.forEach(p => {
    const item = document.createElement("li");
    const state = p.count >= p.qty ? " done" : p.count > 0 ? " partial" : "";
    item.className = "part" + state + (p.code === highlightCode ? " just-read" : "");
    item.innerHTML = `<div><div class="name"></div><div class="code"></div></div><div class="qty">${p.count} / ${p.qty}</div>`;
    item.querySelector(".name").textContent = p.name;
    item.querySelector(".code").textContent = TEXT.code + " " + p.code;
    list.appendChild(item);
  });
  return total > 0 && done === total;
}

function addHistory(text, cssClass) {
  const list = $("historyList");
  const item = document.createElement("li");
  item.className = cssClass;
  item.textContent = new Date().toLocaleTimeString() + "  " + text;
  list.prepend(item);
  while (list.children.length > 30) list.lastChild.remove();
}

// ---- sound ----
let audioContext;
function beep(frequency, duration, delay = 0) {
  try {
    audioContext = audioContext || new (window.AudioContext || window.webkitAudioContext)();
    const osc = audioContext.createOscillator();
    const gain = audioContext.createGain();
    osc.frequency.value = frequency;
    osc.connect(gain);
    gain.connect(audioContext.destination);
    const start = audioContext.currentTime + delay;
    gain.gain.setValueAtTime(0.25, start);
    gain.gain.exponentialRampToValueAtTime(0.001, start + duration);
    osc.start(start);
    osc.stop(start + duration);
  } catch (e) {}
}

// ---- full-screen alerts ----
function showAlert(type, title, text) {
  alertOpen = true;
  $("alert").className = "alert open " + type;
  $("alertTitle").textContent = title;
  $("alertText").textContent = text;
  $("alertButton").textContent = type === "complete" ? TEXT.newBox : TEXT.continue;
}

$("alertButton").onclick = () => {
  const wasComplete = $("alert").classList.contains("complete");
  $("alert").className = "alert";
  alertOpen = false;
  if (wasComplete) startNewBox();
};

// ---- QR handling ----
function findPart(qrData) {
  // Pick the longest code that the QR content starts with.
  let match = null;
  parts.forEach(p => {
    const matches = qrData === p.code || qrData.startsWith(p.code);
    if (matches && (!match || p.code.length > match.code.length)) match = p;
  });
  return match;
}

function handleQr(qrData) {
  if (alertOpen || readCodes.has(qrData)) return; // the same label is never counted twice
  readCodes.add(qrData);

  const part = findPart(qrData);
  if (!part) {
    addHistory(TEXT.unknownLog + qrData, "bad");
    beep(220, 0.5);
    showAlert("error", TEXT.stop, TEXT.notInBox + qrData);
    return;
  }
  if (part.count >= part.qty) {
    addHistory(TEXT.extraLog + part.name + " (" + qrData + ")", "bad");
    beep(220, 0.5);
    showAlert("error", TEXT.stop, TEXT.extraPart.replace("{name}", part.name));
    return;
  }

  part.count++;
  addHistory(part.name + " (" + qrData + ")", "good");
  beep(1200, 0.12);
  if (render(part.code)) {
    beep(880, 0.15, 0.15); beep(1100, 0.15, 0.3); beep(1320, 0.3, 0.45);
    showAlert("complete", TEXT.boxComplete, TEXT.boxCompleteText);
  }
}

function startNewBox() {
  parts.forEach(p => p.count = 0);
  readCodes.clear();
  $("historyList").innerHTML = "";
  render();
}

// ---- camera loop: fetch a frame from /capture and look for a QR code ----
const canvas = $("canvas");
const ctx = canvas.getContext("2d", { willReadFrequently: true });
const frameBox = $("frame");
let frameCount = 0;
let lastFpsTime = performance.now();

function drawQrOutline(location) {
  ctx.strokeStyle = "#1f9d55";
  ctx.lineWidth = 5;
  ctx.beginPath();
  ctx.moveTo(location.topLeftCorner.x, location.topLeftCorner.y);
  ctx.lineTo(location.topRightCorner.x, location.topRightCorner.y);
  ctx.lineTo(location.bottomRightCorner.x, location.bottomRightCorner.y);
  ctx.lineTo(location.bottomLeftCorner.x, location.bottomLeftCorner.y);
  ctx.closePath();
  ctx.stroke();
}

async function cameraLoop() {
  try {
    const response = await fetch("/capture?t=" + Date.now(), { cache: "no-store" });
    if (!response.ok) throw new Error("HTTP " + response.status);
    const bitmap = await createImageBitmap(await response.blob());
    if (canvas.width !== bitmap.width) { canvas.width = bitmap.width; canvas.height = bitmap.height; }
    ctx.drawImage(bitmap, 0, 0);

    const image = ctx.getImageData(0, 0, canvas.width, canvas.height);
    const qr = window.jsQR ? jsQR(image.data, image.width, image.height, { inversionAttempts: "dontInvert" }) : null;
    if (qr && qr.data) {
      drawQrOutline(qr.location);
      frameBox.classList.add("detected");
      handleQr(qr.data.trim());
    } else {
      frameBox.classList.remove("detected");
    }

    $("connection").textContent = window.jsQR ? TEXT.connected : TEXT.qrLibMissing;
    frameCount++;
    const now = performance.now();
    if (now - lastFpsTime > 1000) { $("fps").textContent = frameCount; frameCount = 0; lastFpsTime = now; }
    setTimeout(cameraLoop, 0);
  } catch (e) {
    $("connection").textContent = TEXT.cameraDown;
    setTimeout(cameraLoop, 800);
  }
}

// ---- buttons ----
$("newBoxButton").onclick = startNewBox;
$("editButton").onclick = () => { $("listText").value = serializeList(); $("editDialog").showModal(); };
$("cancelButton").onclick = () => $("editDialog").close();
$("saveButton").onclick = () => {
  parseList($("listText").value);
  saveList();
  $("editDialog").close();
  startNewBox();
};
$("resolution").onchange = e => fetch("/resolution?size=" + e.target.value);

function makeSerial(n) { return String(n).padStart(SERIAL_DIGITS, "0"); }

$("labelsButton").onclick = () => {
  const container = $("labels");
  container.innerHTML = "";
  let serial = 1;
  const labels = [];
  parts.forEach(p => { for (let i = 0; i < p.qty; i++) labels.push([p.code + makeSerial(serial++), p.name]); });
  if (parts[0]) labels.push([parts[0].code + makeSerial(serial++), parts[0].name + TEXT.extraSuffix]);
  labels.push(["99999" + makeSerial(serial++), TEXT.unknownLabel]);

  labels.forEach(([data, name]) => {
    const label = document.createElement("div");
    label.className = "label";
    const qrHolder = document.createElement("div");
    label.appendChild(qrHolder);
    label.appendChild(document.createTextNode(name));
    label.appendChild(document.createElement("br"));
    label.appendChild(document.createTextNode(data));
    container.appendChild(label);
    if (window.QRCode) new QRCode(qrHolder, { text: data, width: 140, height: 140, correctLevel: QRCode.CorrectLevel.M });
  });
  $("labelDialog").showModal();
};
$("closeLabelsButton").onclick = () => $("labelDialog").close();
$("printButton").onclick = () => window.print();

applyStaticText();
loadList();
render();
cameraLoop();
</script>
</body>
</html>
)rawliteral";
