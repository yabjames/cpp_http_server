import http from "k6/http";
import { check, sleep } from "k6";

export const options = {
  thresholds: {
    http_req_failed: [{ threshold: "rate<0.01", abortOnFail: true }], // availability threshold for error rate
    http_req_duration: ["p(99)<1000"], // Latency threshold for percentile
  },
  // define scenarios
  scenarios: {
    breaking: {
      executor: "ramping-vus",
      stages: [
        { duration: "10s", target: 20 },
        { duration: "10s", target: 40 },
        { duration: "10s", target: 80 },
        { duration: "10s", target: 320 },
        { duration: "10s", target: 1000 },
        { duration: "10s", target: 5000 },
        //....
      ],
    },
  },
};

export default function () {
  const res = http.get("http://localhost:3490/");
  check(res, {
    "response code was 200": (res) => res.status == 200,
  });
}
