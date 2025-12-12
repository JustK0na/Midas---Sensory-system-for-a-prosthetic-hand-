import QtQuick 2.15

Rectangle {
    id: root
    width: 900
    height: 420
    color: "#ffffff"

    property int windowSize: 200            // how many points visible on X
    property int maxStored: 20000           // how many points to keep internally per sensor
    property real paddingLeft: 60
    property real paddingRight: 10
    property real paddingTop: 10
    property real paddingBottom: 40

    // store points keyed by sensor id: { "0": [ {x:..., y:...}, ... ], "1": [...] }
    property var pointsMap: ({})
    // expected sensor ids (you can add more colors if you expect more sensors)
    property var sensorColors: ["#1f77b4", "#ff7f0e", "#2ca02c", "#d62728"]
    // fixed Y range example (you can change per your sensors)
    property real ymin: 9000
    property real ymax: 11000

    Canvas {
        id: canvas
        anchors.fill: parent
        onPaint: {
            var ctx = getContext("2d");
            ctx.reset();
            // background
            ctx.fillStyle = "#ffffff";
            ctx.fillRect(0, 0, width, height);

            var left = paddingLeft;
            var right = width - paddingRight;
            var top = paddingTop;
            var bottom = height - paddingBottom;
            var w = right - left;
            var h = bottom - top;

            // axes
            ctx.strokeStyle = "#888";
            ctx.lineWidth = 1;
            ctx.beginPath(); ctx.moveTo(left, bottom); ctx.lineTo(right, bottom); ctx.stroke(); // X
            ctx.beginPath(); ctx.moveTo(left, top); ctx.lineTo(left, bottom); ctx.stroke(); // Y

            // Y ticks and labels
            ctx.fillStyle = "#000";
            ctx.font = "12px sans-serif";
            var ticks = 5;
            for (var t = 0; t <= ticks; ++t) {
                var frac = t / ticks;
                var yval = ymax - frac * (ymax - ymin);
                var ypix = top + frac * h;
                ctx.fillText(yval.toFixed(2), 4, ypix + 4);
                ctx.beginPath(); ctx.moveTo(left - 5, ypix); ctx.lineTo(left, ypix); ctx.stroke();
            }

            // helper mapping functions
            function mapX(x, xmin, xmax) {
                if (xmax === xmin) return left + w/2;
                return left + ((x - xmin) / (xmax - xmin)) * w;
            }
            function mapY(y) {
                if (ymax === ymin) return top + h/2;
                return top + ((ymax - y) / (ymax - ymin)) * h;
            }

            // draw series for each sensor id
            var sensorIds = Object.keys(pointsMap);
            if (sensorIds.length === 0) return;

            // compute a common X window based on *latest* points across all sensors
            var latestX = -Infinity;
            for (var si = 0; si < sensorIds.length; ++si) {
                var arr = pointsMap[sensorIds[si]];
                if (arr && arr.length > 0) {
                    var lx = arr[arr.length - 1].x;
                    if (lx > latestX) latestX = lx;
                }
            }
            var xmax = latestX;
            var xmin = Math.max(0, latestX - (windowSize - 1));

            // draw each sensor's polyline
            for (var idx = 0; idx < sensorIds.length; ++idx) {
                var id = sensorIds[idx];
                var pts = pointsMap[id];
                if (!pts || pts.length === 0) continue;

                var color = sensorColors[id % sensorColors.length] || "#000000";
                ctx.strokeStyle = color;
                ctx.lineWidth = 2;
                ctx.beginPath();
                var started = false;
                for (var j = 0; j < pts.length; ++j) {
                    var p = pts[j];
                    if (p.x < xmin) continue; // skip old
                    var px = mapX(p.x, xmin, xmax);
                    var py = mapY(p.y);
                    if (!started) { ctx.moveTo(px, py); started = true; }
                    else ctx.lineTo(px, py);
                }
                ctx.stroke();

                // label: draw sensor id and last value
                var last = pts[pts.length - 1];
                ctx.fillStyle = color;
                ctx.fillText("Sensor " + id + ": " + last.y.toFixed(3), left + 6, top + 14 + idx * 14);
            }
        }
    }

    // bridge connection: receives newPoint(sensorId, x, y) from C++
    Connections {
        target: bridge
        function onNewPoint(sensorId, x, y) {
            // ensure array exists
            var key = String(sensorId);
            if (!pointsMap[key]) pointsMap[key] = [];
            pointsMap[key].push({x: x, y: y});
            // trim
            if (pointsMap[key].length > maxStored) {
                var remove = pointsMap[key].length - maxStored;
                pointsMap[key].splice(0, remove);
            }
            canvas.requestPaint();
        }
    }
}
