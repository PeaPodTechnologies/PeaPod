# ---- deps (Yarn) ----
FROM --platform=$BUILDPLATFORM node:20-bookworm-slim AS build
WORKDIR /app

# Tools needed for node-gyp native addons (like epoll)
RUN apt-get update && apt-get install -y --no-install-recommends \
    python3 make g++ \
  && rm -rf /var/lib/apt/lists/*

RUN corepack enable

COPY . .

RUN yarn buildapi
RUN npx next build

RUN rm -f /app/.next/standalone/server.js
RUN rm -rf /app/.next/standalone/node_modules
RUN rm -f /app/.next/standalone/package.json

FROM scratch AS export

COPY --from=build /app/.next/standalone /
COPY --from=build /app/.next/static /.next/static
COPY --from=build /app/public /public
COPY --from=build /app/api/*.mjs /api/
COPY --from=build /app/server.mjs /
COPY --from=build /app/microcontroller /microcontroller
COPY --from=build /app/.env /.env
COPY --from=build /app/node_modules /node_modules
COPY --from=build /app/package.json /package.json
COPY --from=build /app/yarn.lock /yarn.lock