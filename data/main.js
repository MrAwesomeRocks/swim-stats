if (!!window.EventSource) {
  var source = new EventSource("/events");

  source.addEventListener(
    "open",
    function (e) {
      console.log("Events Connected");
    },
    false
  );

  source.addEventListener(
    "error",
    function (e) {
      if (e.target.readyState != EventSource.OPEN) {
        console.log("Events Disconnected");
      }
    },
    false
  );

  source.addEventListener(
    "message",
    function (e) {
      console.log("message", e.data);

      let elem = document.createElement("span");
      elem.innerHTML = e.data;
      document.appendChild(elem);
    },
    false
  );

  source.addEventListener(
    "mpuData",
    function (e) {
      console.log("mpuData", e.data);

      const data = JSON.parse(e.data);

      let ypr = document.getElementById("ypr");
      if (ypr !== null) {
        ypr.innerHTML = `YPR: ${data.ypr[0]}, ${data.ypr[1]}, ${data.ypr[2]}`;
      }

      let accel = document.getElementById("accel");
      if (accel !== null) {
        accel.innerHTML = `Accel: ${data.accel[0]}, ${data.accel[1]}, ${data.accel[2]}`;
      }

      let temp = document.getElementById("temp");
      if (temp !== null) {
        temp.innerHTML = `Temp: ${data.temp}`
      }
    },
    false
  );
}
