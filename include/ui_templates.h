/**
 * @file ui_templates.h
 * @brief HTML/CSS/JS templates for the web interface
 * 
 * Stores UI content as const strings to minimize memory usage.
 * Uses minimal, optimized HTML/CSS/JS for fast loading.
 */

#ifndef UI_TEMPLATES_H
#define UI_TEMPLATES_H

/**
 * @brief Main HTML page template
 * 
 * Placeholders:
 *   %s - IP address
 *   %d - Relay 1 state (0 or 1)
 *   %d - Relay 2 state (0 or 1)
 *   %d - Relay 3 state (0 or 1)
 *   %d - Relay 4 state (0 or 1)
 */
static const char HTML_PAGE[] = 
"<!DOCTYPE html>"
"<html><head>"
"<meta charset='UTF-8'>"
"<meta name='viewport' content='width=device-width,initial-scale=1'>"
"<title>Relay Control</title>"
"<style>"
"*{box-sizing:border-box;margin:0;padding:0}"
"body{font-family:system-ui,-apple-system,sans-serif;background:#1a1a2e;color:#eee;min-height:100vh;padding:20px}"
"h1{text-align:center;margin-bottom:20px;color:#0f0}"
".info{text-align:center;color:#888;margin-bottom:10px;font-size:14px}"
".resp{text-align:center;color:#0f0;margin-bottom:20px;font-size:12px;height:16px}"
".grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(150px,1fr));gap:15px;max-width:600px;margin:0 auto}"
".card{background:#16213e;border-radius:12px;padding:20px;text-align:center;transition:transform .2s}"
".card:active{transform:scale(.95)}"
".name{font-size:14px;color:#888;margin-bottom:10px}"
".btn{width:100%%;padding:15px;border:none;border-radius:8px;font-size:16px;font-weight:bold;cursor:pointer;transition:all .2s}"
".btn.on{background:#00ff88;color:#000}"
".btn.off{background:#333;color:#888}"
".btn:disabled{opacity:.5;cursor:wait}"
".all{margin-top:20px;display:flex;gap:10px;justify-content:center}"
".all button{padding:10px 20px;border:none;border-radius:6px;cursor:pointer;font-weight:bold}"
".all-on{background:#00ff88;color:#000}"
".all-off{background:#ff4444;color:#fff}"
"</style>"
"</head><body>"
"<h1>⚡ Relay Control</h1>"
"<p class='info'>IP: %s</p>"
"<p class='resp' id='resp'>Response: --</p>"
"<div class='grid'>"
"<div class='card'><div class='name'>💡 Light 1</div><button class='btn %s' id='r0' onclick='toggle(0)'>%s</button></div>"
"<div class='card'><div class='name'>💡 Light 2</div><button class='btn %s' id='r1' onclick='toggle(1)'>%s</button></div>"
"<div class='card'><div class='name'>🌀 Fan 1</div><button class='btn %s' id='r2' onclick='toggle(2)'>%s</button></div>"
"<div class='card'><div class='name'>🌀 Fan 2</div><button class='btn %s' id='r3' onclick='toggle(3)'>%s</button></div>"
"</div>"
"<div class='all'>"
"<button class='all-on' onclick='allOn()'>All ON</button>"
"<button class='all-off' onclick='allOff()'>All OFF</button>"
"</div>"
"<script>"
"const respEl=document.getElementById('resp');"
"function showTime(ms){respEl.textContent='Response: '+ms+'ms';respEl.style.color=ms<100?'#0f0':ms<300?'#ff0':'#f44';}"
"async function toggle(id){"
"const btn=document.getElementById('r'+id);"
"btn.disabled=true;"
"const t0=performance.now();"
"try{"
"const r=await fetch('/relay/'+id+'/toggle');"
"const d=await r.json();"
"showTime(Math.round(performance.now()-t0));"
"btn.className='btn '+(d.state?'on':'off');"
"btn.textContent=d.state?'ON':'OFF';"
"}catch(e){console.error(e);respEl.textContent='Error';}"
"btn.disabled=false;"
"}"
"async function allOn(){"
"const t0=performance.now();"
"await fetch('/relay/all/on');"
"showTime(Math.round(performance.now()-t0));"
"setTimeout(()=>location.reload(),500);"
"}"
"async function allOff(){"
"const t0=performance.now();"
"await fetch('/relay/all/off');"
"showTime(Math.round(performance.now()-t0));"
"setTimeout(()=>location.reload(),500);"
"}"
"</script>"
"</body></html>";

/**
 * @brief JSON response template for relay status
 * 
 * Placeholders:
 *   %d - Relay ID
 *   %s - Relay name
 *   %d - State (0 or 1)
 */
static const char JSON_RELAY_STATUS[] = 
"{\"id\":%d,\"name\":\"%s\",\"state\":%d}";

/**
 * @brief JSON response template for all relays status
 */
static const char JSON_ALL_STATUS_START[] = "{\"relays\":[";
static const char JSON_ALL_STATUS_END[] = "]}";

/**
 * @brief JSON error response
 */
static const char JSON_ERROR[] = "{\"error\":\"%s\"}";

/**
 * @brief JSON success response
 */
static const char JSON_SUCCESS[] = "{\"success\":true,\"message\":\"%s\"}";

#endif // UI_TEMPLATES_H
