#include "web.h"
#include "minilogo.h"

static const char OSM_INDEX[] = OSM_HTML_BEGIN		 \
  "<h1>Open Sound Module</h1>"				 \
  /*  "<p><a href='/scan'>Scan Networks</a></p>"	 \ */
  "<p><a href='/auth'>Add WiFi Credentials</a></p>"	 \
  "<p><a href='/settings'>Network Settings</a></p>"	 \
  "<p><a href='/address'>Address Mapping</a></p>"	 \
  "<p><a href='/ranges'>Value Ranges</a></p>"		 \
  "<p><a href='/cvgate'>Control</a></p></ul>"		 \
  "<p><a href='/status'>Status</a></p></ul>"		 \
  "<p><a href='/about'>About</a></p>"			 \
  OSM_HTML_END;

static const char OSM_HELLO[] = OSM_HTML_BEGIN "<h1>Hello!</h1>" OSM_HTML_END;

static const char OSM_CVGATE[] = OSM_HTML_BEGIN				\
  "<h1>CV/Gate</h1>"							\
  "<input type='range' id='cva' min='0' max='4095''/>"			\
  "<br><button id='tra'>A</button>"					\
  "<br><input type='range' id='cvb' min='0' max='4095'/>"		\
  "<br><button id='trb'>B</button>"					\
  "<script>"								\
  "function cv(i,v){var rq=new XMLHttpRequest();rq.open('GET', '/cvo'+i+'?v='+v);rq.send(null);};" \
  "function tr(i){var rq=new XMLHttpRequest();rq.open('GET', '/tro'+i);rq.send(null);};" \
  "document.getElementById('cva').onchange=function(){cv(0,this.value)};" \
  "document.getElementById('cvb').onchange=function(){cv(1,this.value)};" \
  "document.getElementById('tra').onclick=function(){tr(0)};"		\
  "document.getElementById('trb').onclick=function(){tr(1)};"		\
  "</script>"					\
  OSM_HTML_BACK OSM_HTML_END;

static const char OSM_ABOUT[] = OSM_HTML_BEGIN				\
  "<h1>About</h1>"							\
  "<h2>Open Sound Module</h2>"						\
  "<p>WiFi OSC to CV/Gate interface</p>"				\
  "<p>Firmware v0.1</p>"						\
  "<p>See our <a href='http://www.rebeltech.org/products/open-sound-module/'>website</a> for instructions and information.</p>"	\
  "<br><img src='logo.png'/><br>"					\
  OSM_HTML_BACK								\
  OSM_HTML_END;

static const char OSM_STYLE[] = 
  "html{height:100%;margin:auto;background-color:white}"		\
  "body{box-sizing:border-box;min-height:80%;padding:20px;background-color:#84AFF4;font-family:'Lucida Sans Unicode','Lucida Grande',sans-serif;font-weight:normal;color:white;margin:4px auto;max-width:380px;text-align:center;border:3px solid #6f8bb8;border-radius:5px}" \
  "a{color:#f0f8ff}"							\
  "div,h1{margin:25px}"							\
  "input,button{margin:10px;height:30px}"				\
  "button,select{border-color:#6f8bb8;background-color:#5f7eb0;color:white;border-radius:5px;font-size:15px;font-weight:bold}" \
  "input[type='text'],input[type='password']{background-color:white;color:#6e6e70;border-color:white;border-radius:5px;height:25px;text-align:center;font-size:15px}" \
  "input[type='radio']{position:relative;bottom:-0.33em;border:0;height:1.5em;width:15%}";

START_OF_HTTP_PAGE_DATABASE(osm_http_pages)
    ROOT_HTTP_PAGE_REDIRECT("/index.html"),
    { "/index.html", "text/html", WICED_STATIC_URL_CONTENT, 
	.url_content.static_data = {OSM_INDEX, sizeof(OSM_INDEX)-1 }},
    { "/style.css", "text/css", WICED_STATIC_URL_CONTENT, 
    .url_content.static_data = {OSM_STYLE, sizeof(OSM_STYLE)-1 }},
    { "/hello", "text/html", WICED_STATIC_URL_CONTENT, 
	.url_content.static_data = {OSM_HELLO, sizeof(OSM_HELLO)-1 }},
    { "/about", "text/html", WICED_STATIC_URL_CONTENT, 
	.url_content.static_data = {OSM_ABOUT, sizeof(OSM_ABOUT)-1 }},
    { "/status", "text/html", WICED_DYNAMIC_URL_CONTENT, 
	.url_content.dynamic_data = {process_status, 0 }, },
    { "/settings", "text/html", WICED_DYNAMIC_URL_CONTENT, 
	.url_content.dynamic_data = {process_settings, 0 }, },
    { "/ranges", "text/html", WICED_DYNAMIC_URL_CONTENT, 
	.url_content.dynamic_data = {process_ranges, 0 }, },
    { "/address", "text/html", WICED_DYNAMIC_URL_CONTENT, 
	.url_content.dynamic_data = {process_address, 0 }, },
    { "/auth", "text/html", WICED_DYNAMIC_URL_CONTENT, 
	.url_content.dynamic_data = {process_auth, 0 }, },
#if 0
    { "/scan", "text/html", WICED_DYNAMIC_URL_CONTENT, 
	.url_content.dynamic_data = {process_scan, 0 }, },
#endif
    { "/reconnect_ap", "text/html", WICED_DYNAMIC_URL_CONTENT, 
	.url_content.dynamic_data = {process_reconnect, 1 }, },
    { "/reconnect_sta", "text/html", WICED_DYNAMIC_URL_CONTENT, 
	.url_content.dynamic_data = {process_reconnect, 2 }, },
    { "/save_net", "text/html", WICED_DYNAMIC_URL_CONTENT, 
	.url_content.dynamic_data = {process_save, 1 }, },
    { "/save_osc", "text/html", WICED_DYNAMIC_URL_CONTENT, 
	.url_content.dynamic_data = {process_save, 2 }, },
    { "/save_range", "text/html", WICED_DYNAMIC_URL_CONTENT, 
	.url_content.dynamic_data = {process_save, 3 }, },
    { "/reset0", "text/html", WICED_DYNAMIC_URL_CONTENT, 
	.url_content.dynamic_data = {process_reset, 0 }, },
    { "/reset1", "text/html", WICED_DYNAMIC_URL_CONTENT, 
	.url_content.dynamic_data = {process_reset, 1 }, },
    { "/reset2", "text/html", WICED_DYNAMIC_URL_CONTENT, 
	.url_content.dynamic_data = {process_reset, 2 }, },
    { "/reset3", "text/html", WICED_DYNAMIC_URL_CONTENT, 
	.url_content.dynamic_data = {process_reset, 3 }, },
    { "/reset99", "text/html", WICED_DYNAMIC_URL_CONTENT, 
	.url_content.dynamic_data = {process_reset, 99 }, },
    { "/logo.png", "image/png", WICED_STATIC_URL_CONTENT, 
	.url_content.static_data = {minilogo_png, sizeof(minilogo_png) }},
    { "/cvgate", "text/html", WICED_STATIC_URL_CONTENT, 
	.url_content.static_data = {OSM_CVGATE, sizeof(OSM_CVGATE)-1 }},
    { "/cvo0", "application/json", WICED_DYNAMIC_URL_CONTENT, 
	.url_content.dynamic_data = {process_cvout, 0 }, },
    { "/cvo1", "application/json", WICED_DYNAMIC_URL_CONTENT, 
	.url_content.dynamic_data = {process_cvout, 1 }, },
    { "/tro0", "application/json", WICED_DYNAMIC_URL_CONTENT, 
	.url_content.dynamic_data = {process_trout, 0 }, },
    { "/tro1", "application/json", WICED_DYNAMIC_URL_CONTENT, 
	.url_content.dynamic_data = {process_trout, 1 }, },
END_OF_HTTP_PAGE_DATABASE();

