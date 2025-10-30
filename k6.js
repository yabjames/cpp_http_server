import http from "k6/http";
import { check, sleep } from "k6";

export const options = {
  vus: 1000, // 10 virtual users
  duration: "10s", // Test duration of 30 seconds
  thresholds: {
    http_req_duration: ["p(95)<200"], // 95% of requests should be below 200ms
    "http_req_failed{status:400}": ["rate<0.01"], // less than 1% of 400 status codes
  },
};

export default function () {
  const res = http.get("http://localhost:3490/");
  check(res, {
    "status is 200": (r) => r.status === 200,
    // 'body contains "success"': (r) => r.body.includes("success"),
  });
  sleep(1); // Simulate user think time
}
