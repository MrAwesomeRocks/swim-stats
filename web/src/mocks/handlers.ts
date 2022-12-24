import { ResponseTransformer, rest } from "msw";

export const handlers = [
  // SSE
  rest.get("/events", (req, res, ctx) => {
    console.log("Mock SSE server hit");

    let messages: ResponseTransformer<any, any>[] = [];
    const data = {
      ypr: [Math.random(), Math.random(), Math.random()],
      accel: [Math.random(), Math.random(), Math.random()],
      temp: Math.random(),
    };
    messages.push(
      ctx.delay(1000),
      ctx.body(
        `event: mpuData\ndata: ${JSON.stringify(data)}\nid: ${Date.now()}\n\n`
      )
    );

    return res(
      ctx.status(200),
      ctx.set("Connection", "keep-alive"),
      ctx.set("Content-Type", "text/event-stream"),
      ...messages
    );
  }),
];
