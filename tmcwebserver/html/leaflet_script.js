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

var markerGroup = L.layerGroup().addTo(map);
var overlayGroup = L.layerGroup().addTo(map);

//request
function search() {
	// clean everything
	document.getElementById("not-found").style.display = "none";
	document.getElementById("server-down").style.display = "none";
	markerGroup.clearLayers();

	if (slider) {
		var remove_slider = document.getElementsByClassName("leaflet-timeline-control")[0];
		remove_slider.parentNode.removeChild(remove_slider);
		slider = null;
	}

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
		} else if (xhr.readyState === 4) {
			document.getElementById("server-down").style.display = "block";
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

function addJson(json) {
	var select_visual = document.getElementById("visual");
	var visual = select_visual.options[select_visual.selectedIndex].value;

	if (visual == "timeline") {
		geodata = createGeoJson(json);
		timeline = L.timeline(geodata, {
			style: function(geodata){
				return {
					// stroke: false,
					weight: 5,
					color: getColorFor(geodata.properties.name),
					fillOpacity: 0.5
					}
				},
			pointToLayer: function(geoJsonPoint, latlng) {
					return L.circle(latlng,{
						radius: 75,
						weight: 0.2
					});
			},
			showTicks : true,
			onEachFeature: function(feature, layer) {
				layer.bindTooltip(feature.properties.name);
			}
		});

		if (!slider) {
			addTimeLineControl();
		} else {
			slider.addTo(map);
		}
		slider.addTimelines(timeline);
		timeline.addTo(markerGroup);
	} else if (visual == "heatmap") {
		data = createHeatList(json);
		console.log(data);
		var heat = L.heatLayer(data, {radius: 20}).addTo(markerGroup);
	}
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
					"type": "Point",
					"coordinates": json["events"][i]["path"][0]
				}
			});
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

function createHeatList(json) {
	// console.log("answer: " + JSON.stringify(json));
	events = []
	for (var i = 0; i < json["events"].length; i++) {
		// create heatmap-feature
		if (json["events"][i]["path"].length == 1) {
			events.push([json["events"][i]["path"][0][1], json["events"][i]["path"][0][0], 0.2]);
		} else {
			for (var j = 0; j < json["events"][i]["path"].length; j++) {
				events.push([json["events"][i]["path"][j][1], json["events"][i]["path"][j][0], 0.2]);
			}
		}
	}
	return events;
}

function addTimeLineControl() {
	if (slider != null) {
	}
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

function overlay() {
	if (isOverlayPointsDrawn && isOverlayLinesDrawn){
		overlayGroup.clearLayers();
		isOverlayPointsDrawn = false;
		isOverlayLinesDrawn = false;
		console.log("overlay removed");
		return;
	}
	overlay_lines();
	overlay_points();
}

function overlay_points() {
	var xhr_p = new XMLHttpRequest();
	xhr_p.open("GET", url + "points.json", true);
	xhr_p.setRequestHeader("Content-type", "application/json");
	xhr_p.onreadystatechange = function () {
		if (xhr_p.readyState === 4 && xhr_p.status === 200) {
			var json = JSON.parse(xhr_p.responseText);
			DrawOverlayPoints(json);
		} else if (xhr.readyState === 4) {
			document.getElementById("server-down").style.display = "block";
		}
	};
	xhr_p.send();
}
function DrawOverlayPoints(json) {
	if (!isOverlayPointsDrawn) {
		isOverlayPointsDrawn = true;
		for (var i = 0; i < json.length; i++) {
			L.circle(json[i][1], {
				radius: 50,
				weight: 0.2,
				color: "#f0027f"
			}).addTo(overlayGroup).bindPopup("" + json[i][0]);
		}
	}
	console.log("done painting points");
}

function overlay_lines() {
	var xhr_o = new XMLHttpRequest();
	xhr_o.open("GET", url + "poffsets.json", true);
	xhr_o.setRequestHeader("Content-type", "application/json");
	xhr_o.onreadystatechange = function () {
		if (xhr_o.readyState === 4 && xhr_o.status === 200) {
			var json = JSON.parse(xhr_o.responseText);
			DrawOverlayLines(json);
		} else if (xhr.readyState === 4) {
			document.getElementById("server-down").style.display = "block";
		}
	};
	xhr_o.send();
}
function DrawOverlayLines(json) {
	if (!isOverlayLinesDrawn) {
		isOverlayLinesDrawn = true;
		for (var i = 0; i < json.length; i++) {
			L.polyline(json[i], {
				weight: 2,
				color: "#386cb0"
			}).addTo(overlayGroup);
		}
	}
	console.log("done painting lines");
}

function hideNotFound() {
	var x = document.getElementById("not-found");
	if (x.style.display === "block") {
		x.style.display = "none";
	}
}
function hideServerDown() {
	var x = document.getElementById("server-down");
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
		} else if (xhr_p.readyState === 4) {
			document.getElementById("server-down").style.display = "block";
		}
	};
	xhr_p.send();
}
function setDateRange(json) {
	var min = json['min'];
	var max = json['max'];
	console.log("min: " + min + " max: " + max);
	document.getElementById("start-date").setAttribute('min', min);
	document.getElementById("start-date").setAttribute('max', max);
	document.getElementById("end-date").setAttribute('min', min);
	document.getElementById("end-date").setAttribute('max', max);
	document.getElementById("start-date").setAttribute('value', min);
	document.getElementById("end-date").setAttribute('value', max);
}
dateRange();
