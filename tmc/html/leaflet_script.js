var bounds = [
    [47.3, 5.9], // Southwest coordinates
    [54.9, 16.9512215]  // Northeast coordinates
];
var map = new L.map('map').setView([50.5, 9.125], 6);//.setMaxBounds(bounds);

L.tileLayer('https://api.tiles.mapbox.com/v4/{id}/{z}/{x}/{y}.png?access_token=pk.eyJ1IjoibWFwYm94IiwiYSI6ImNpejY4NXVycTA2emYycXBndHRqcmZ3N3gifQ.rJcFIG214AriISLbB6B5aw', {
    center: map.getBounds().getCenter(),
    maxZoom: 18,
    minZoom: 2,
    attribution: 'Map data &copy; <a href="http://openstreetmap.org">OpenStreetMap</a> contributors, ' +
    '<a href="http://creativecommons.org/licenses/by-sa/2.0/">CC-BY-SA</a>, ' +
    'Imagery © <a href="http://mapbox.com">Mapbox</a>',
    id: 'mapbox.streets',
    zoom: 7,
    maxBoundsViscosity: 1.0
}).addTo(map);

map.doubleClickZoom.disable();
var lat, lat_goal, lat_start, lon, lon_start, lon_goal, lat_start_short, lat_goal_short, lon_start_short, lon_goal_short;
var geojsonFeature;
var geoObjectName;

var layerlist = {};
map.on('click', onMapClick);
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

function onMapClick(e){
    lat = e.latlng.lat;
    lon = e.latlng.lng;
    var popup = L.popup()
        .setLatLng(e.latlng)
        .setContent("here click")
        .openOn(map);
}
//request
function search() {
    console.log(map.getBounds());
    var xhr = new XMLHttpRequest();
    var url = "http://localhost:8081/api/route";
    xhr.open("POST",url,true);
    xhr.setRequestHeader("Content-type", "application/json");
    xhr.onreadystatechange = function () {
        if (xhr.readyState === 4 && xhr.status === 200) {
            var json = JSON.parse(xhr.responseText);
            createGeoJson(json);
        }
    };
    var myObj = {"route": [ [lon_start,lat_start], [lon_goal,lat_goal] ]};
    var data = JSON.stringify(myObj);
    console.log("request: " + data);
    xhr.send(data);
}
//Called on ButtonClick [Draw Route]. Calls function sendData and deletes the existing Route.
function drawRoute() {
    sendData();
    if (isRouteDrawn) {
        map.removeLayer(layerlist["line"]);
    }
}
//Called from sendData. Creates Geojsonobject for the received Route.
function createGeoJson(json) {
	console.log("answer: " + json.route);
	if (isGoalDrawn && isStartDrawn) {
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
    }
	console.log("done painting");
}
//Infopannel in the topright corner.
var info = L.control();
info.onAdd = function (map) {
    this._div = L.DomUtil.create('div', 'info'); // create a div with a class "info"
    L.DomEvent.disableClickPropagation(this._div);
    this.update();
    return this._div;
};
info.update = function (props) {
    this._div.innerHTML = '<h4>Search</h4>'
        + '<div class="start">Start: </div>'
        + '<input type="datetime-local" id="end-time" name="party-time" value="2018-06-12T19:30" min="2018-06-07T00:00" max="2018-06-14T00:00"  required />'
        + '<br> <div class="end"> End: </div>'
        + '<input type="datetime-local" id="end-time" name="party-time" value="2018-06-12T19:30" min="2018-06-07T00:00" max="2018-06-14T00:00"  required />'
        + '<br>'
        + '<button class="btn3"onclick="search()">search</button>';
};
info.addTo(map);
