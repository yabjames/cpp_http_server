import http from "k6/http";
import { check } from "k6";

export const options = {
  thresholds: {
    http_req_failed: [{ threshold: "rate<0.01", abortOnFail: true }], // availability threshold for error rate
    http_req_duration: ["p(99)<1000"], // Latency threshold for percentile
  },
  scenarios: {
    constant_request_rate: {
      executor: "constant-arrival-rate",
      rate: 30000,
      timeUnit: "1s", // 1000 iterations per second, i.e. 1000 RPS
      duration: "10s",
      preAllocatedVUs: 100, // how large the initial pool of VUs would be
      maxVUs: 20000, // if the preAllocatedVUs are not enough, we can initialize more
    },
  },
};

export default function () {
  const res = http.post("http://localhost:3490/test2/123/foo/432");
  check(res, {
    "response code was 200": (res) => res.status == 200,
  });
}
