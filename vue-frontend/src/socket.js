import mitt from "mitt";


let host = "ws://" + document.location.host + "/dashws";

if (!import.meta.env.PROD) {
  host = "ws://rst-544b84.sphere.home/dashws"; // For Local Testing via npm run serve
}

const socket = new WebSocket(host);
const emitter = mitt();

const Socket = {
  $on: (...args) => emitter.on(...args),
  $off: (...args) => emitter.off(...args),
  $emit: (...args) => emitter.emit(...args),
  send(message) {
    if (socket.readyState === 1) {
      socket.send(message);
    }
  }
};

socket.onopen = function () {
  Socket.$emit("connected");
};

socket.onclose = function () {
  Socket.$emit("disconnected");
};

socket.onmessage = function (msg) {
  Socket.$emit("message", JSON.parse(msg.data));
};
socket.onerror = function (err) {
  Socket.$emit("error", err);
};


let socketInterval = setInterval(() => {
  if (socket.readyState == 0) {
    Socket.$emit("disconnected");
  } else if (socket.readyState === 3) {
    location.reload();
  } else {
    socket.send("{\"command\":\"ping\"}");
  }
}, 5000);

if (!import.meta.env.PROD) {
  clearInterval(socketInterval);
}

export default Socket;
