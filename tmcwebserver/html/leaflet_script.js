var bounds = [
    [47.2, 5.8], // Southwest coordinates
    [55.1, 15.4]  // Northeast coordinates
    ];
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

map.doubleClickZoom.disable();
var lat, lat_goal, lat_start, lon, lon_start, lon_goal, lat_start_short, lat_goal_short, lon_start_short, lon_goal_short;
var geojsonFeature;
var geoObjectName;
var isOverlayPointsDrawn = false;
var isOverlayLinesDrawn = false;
var points;
var lines;
var url = "";

var layerlist = {};

//Called from SetStart/SetGoal. Creates GeoJson object for the start or goal and adds it to the map.
function createGeo(lat, lon) {
	geojsonFeature = {
		"type": "Feature",
		"properties": {
			"name": geoObjectName,
			"popupContent": "Long: " + lon.toFixed(3) + " Lat: " + lat.toFixed(3) + "<br>",
			"lat": lat,
			"lon": lon
		},
		"geometry": {
			"type": "Point",
			"coordinates": [lon, lat]
		},

	};
	L.geoJson(geojsonFeature, {
		pointToLayer: function (feature, latlng) {
			return L.marker(latlng, {icon: myIcon});
		},
		onEachFeature: function(feature, layer) {
			layerlist[feature.properties.name]=layer;
			if (feature.properties && feature.properties.popupContent) {
				layer.bindPopup(feature.properties.popupContent);
			}
		}
	}).addTo(map);
}

//request
function search() {
	var xhr = new XMLHttpRequest();
	xhr.open("POST", url + "query", true);
	xhr.setRequestHeader("Content-type", "application/json");
	xhr.onreadystatechange = function () {
		if (xhr.readyState === 4 && xhr.status === 200) {
			var json = JSON.parse(xhr.responseText);
			createGeoJson(json);
		}
	};

	console.log(map.getBounds());
	var bounds = map.getBounds();
	var start = document.getElementById("start-time").getAttribute("value");
	var end = document.getElementById("end-time").getAttribute("value");
	var body = {
		"view": {
			"northeast": bounds["_northEast"],
			"southwest": bounds["_southWest"]
		},
		"date": {
			"start": start,
			"end": end
		}
	};
	var data = JSON.stringify(body);
	console.log("request: " + data);
	xhr.send(data);
}
//Called from sendData. Creates Geojsonobject for the received Route.
function createGeoJson(json) {
	console.log("answer: " + json.route);
	/*if (isGoalDrawn && isStartDrawn) {
		isRouteDrawn = true;
		var myLines = [{
			"type": "LineString",
			"properties": {
				"name": "line"
			},
			"coordinates": json.route

		}];
		L.geoJson(myLines, {
			pointToLayer: function (feature, latlng) {
				return L.marker(latlng, {icon: myIcon});
			},
			onEachFeature: function(feature, layer) {
				layerlist[feature.properties.name]=layer;
				if (feature.properties && feature.properties.popupContent) {
					layer.bindPopup(feature.properties.popupContent);
				}
			}
		}).addTo(map);
	}*/
	console.log("done painting");
}

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

function DrawOverlayPoints(json) {
	if (!isOverlayPointsDrawn) {
		isOverlayPointsDrawn = true;
		for (var i = 0; i < json.length; i++) {
			L.circle(json[i][1], {
				radius: 50,
				weight: 0.2,
				color: "#f0027f"
			}).addTo(map).bindPopup("" + json[i][0]);
		}
	}
	console.log("done painting points");
}

function onEachFeature(feature, layer) {
	// does this feature have a property named popupContent?
	if (feature.properties && feature.properties.popupContent) {
		layer.bindPopup(feature.properties.popupContent);
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
