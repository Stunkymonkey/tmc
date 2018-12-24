var url = "";
var timeline;
var slider;

var bounds = [
    [47.2, 5.8], // Southwest coordinates
    [55.1, 15.4]  // Northeast coordinates
    ];

var isOverlayPointsDrawn = false;
var isOverlayLinesDrawn = false;
var points;
var lines;

var map = new L.map('map', {
	preferCanvas: true
}).setView([48.7758, 9.1829], 13).setMaxBounds(bounds);
map.doubleClickZoom.disable();

L.tileLayer('https://api.tiles.mapbox.com/v4/{id}/{z}/{x}/{y}.png?access_token=pk.eyJ1IjoibWFwYm94IiwiYSI6ImNpejY4NXVycTA2emYycXBndHRqcmZ3N3gifQ.rJcFIG214AriISLbB6B5aw', {
	center: map.getBounds().getCenter(),
	maxZoom: 16,
	minZoom: 6,
	attribution: 'Map data &copy; <a href="http://openstreetmap.org">OpenStreetMap</a> contributors, ' +
	'<a href="http://creativecommons.org/licenses/by-sa/2.0/">CC-BY-SA</a>, ' +
	'Imagery © <a href="http://mapbox.com">Mapbox</a>',
	id: 'mapbox.streets',
	zoom: 11,
}).addTo(map);

//request
function search() {
	var x = document.getElementById("not-found");
	x.style.display = "none";
	markerGroup.clearLayers();
	var xhr = new XMLHttpRequest();
	xhr.open("POST", url + "query", true);
	xhr.setRequestHeader("Content-type", "application/json");
	xhr.onreadystatechange = function () {
		if (xhr.readyState === 4 && xhr.status === 200) {
			var json = JSON.parse(xhr.responseText);
			if (json.events != "") {
				addJson(json);
			} else {
				// no events
				var x = document.getElementById("not-found");
				if (x.style.display === "none") {
					x.style.display = "block";
				}
			}
		}
	};

	var bounds = map.getBounds();
	var start = document.getElementById("start-date").value;
	var end = document.getElementById("end-date").value;
	var start_time = document.getElementById("start-time").value;
	var end_time = document.getElementById("end-time").value;
	var body = {
		"view": {
			"northeast": bounds["_northEast"],
			"southwest": bounds["_southWest"]
		},
		"date": {
			"start": start,
			"end": end
		},
		"time": {
			"start": start_time,
			"end": end_time
		}
	};
	var data = JSON.stringify(body);
	// console.log("request: " + data);
	xhr.send(data);
}

function addGeoJson(data) {
	timeline = L.timeline(data, {
		style: function(data){
			return {
				// stroke: false,
				weight: 5,
				color: getColorFor(data.properties.name),
				fillOpacity: 0.5
				}
			},
		pointToLayer: function(geoJsonPoint, latlng) {
				return L.circle(latlng);
		},
		// waitToUpdateMap: true,
		showTicks : true,
		onEachFeature: function(feature, layer) {
			layer.bindTooltip(feature.properties.name);
		}
	});
	slider.addTimelines(timeline);
	timeline.addTo(map);
	console.log("added all events");
}

//Called from search
function createGeoJson(json) {
	// console.log("answer: " + JSON.stringify(json));
	events = []
	for (var i = 0; i < json["events"].length; i++) {
		// create geojson feature: line or marker
		if (json["events"][i]["path"].length == 1) {
			events.push({
				"type": "Feature",
				"properties": {
					"name": json["events"][i]["event"],
					"start": json["events"][i]["start"],
					"end": json["events"][i]["end"]
				},
				"geometry": {
					"type": "Point",
					"coordinates": json["events"][i]["path"][0]
				}
			});
		} else {
			events.push({
				"type": "Feature",
				"properties": {
					"name": json["events"][i]["event"],
					"start": json["events"][i]["start"],
					"end": json["events"][i]["end"]
				},
				"geometry": {
					"type": "LineString",
					"coordinates": json["events"][i]["path"]
				}
			});
		}
	}
	return {"type": "FeatureCollection", "features": events};
}

function addTimeLineControl() {
	slider = L.timelineSliderControl({
		formatOutput: function(date) {
			var result = new Date(date);
			return result.toLocaleDateString() + " " + result.toLocaleTimeString();
		},
		enableKeyboardControls: true,
	});
	slider.addTo(map);
}

function getColorFor(str) {
	var hash = 0;
	for (var i = 0; i < str.length; i++) {
		hash = str.charCodeAt(i) + ((hash << 5) - hash);
	}
	var red = (hash >> 24) & 0xff;
	var grn = (hash >> 16) & 0xff;
	var blu = (hash >>  8) & 0xff;
	return 'rgb(' + red + ',' + grn + ',' + blu + ')';
}

// function earthquakeColor(mag) {
//     return mag > 5 ? '#FE6C00' :
//            mag > 4  ? '#FCB230' :
//            mag > 3 ? '#ECC918' :
//            mag > 2 ? '#FCE46C' :
//            mag > 1   ? '#F6E488' :
//                       '#58D68D';
// }

function overlay() {
	var xhr_o = new XMLHttpRequest();
	xhr_o.open("GET", url + "poffsets.json", true);
	xhr_o.setRequestHeader("Content-type", "application/json");
	xhr_o.onreadystatechange = function () {
		if (xhr_o.readyState === 4 && xhr_o.status === 200) {
			var json = JSON.parse(xhr_o.responseText);
			DrawOverlayLines(json);
		}
	};
	xhr_o.send();
}

function overlay_points() {
	var xhr_p = new XMLHttpRequest();
	xhr_p.open("GET", url + "points.json", true);
	xhr_p.setRequestHeader("Content-type", "application/json");
	xhr_p.onreadystatechange = function () {
		if (xhr_p.readyState === 4 && xhr_p.status === 200) {
			var json = JSON.parse(xhr_p.responseText);
			DrawOverlayPoints(json);
		}
	};
	xhr_p.send();
}

function DrawOverlayLines(json) {
	if (!isOverlayLinesDrawn) {
		isOverlayLinesDrawn = true;
		for (var i = 0; i < json.length; i++) {
			L.polyline(json[i], {
				weight: 2,
				color: "#386cb0"
			}).addTo(map);
		}
	}
	console.log("done painting lines");
	overlay_points();
}

function hideNotFound() {
	var x = document.getElementById("not-found");
	if (x.style.display === "block") {
		x.style.display = "none";
	}
}

function dateRange() {
	var xhr_p = new XMLHttpRequest();
	xhr_p.open("GET", url + "date-range", true);
	xhr_p.setRequestHeader("Content-type", "application/json");
	xhr_p.onreadystatechange = function () {
		if (xhr_p.readyState === 4 && xhr_p.status === 200) {
			var json = JSON.parse(xhr_p.responseText);
			setDateRange(json);
		}
	};
	xhr_p.send();
}

function setDateRange(json) {
	var min = json['min'].split(" ")[0];
	var max = json['max'].split(" ")[0];
	console.log("min: " + min + " max: " + max);
	document.getElementById("start-time").setAttribute('min', min);
	document.getElementById("start-time").setAttribute('max', max);
	document.getElementById("end-time").setAttribute('min', min);
	document.getElementById("end-time").setAttribute('max', max);
	document.getElementById("start-time").setAttribute('value', min);
	document.getElementById("end-time").setAttribute('value', max);
}

dateRange();
addTimeLineControl();
