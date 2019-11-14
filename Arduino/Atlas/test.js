const TICKS_PER_TURN = 2245;
let ticks = 0;
let acumulator = 0;
let multiplier = 0;

function interrupt() {
  for (let i = 0; i < TICKS_PER_TURN*22.6; i++) {
    ticks++;
    ticks = ticks % TICKS_PER_TURN;
    if (ticks == 0) {
      multiplier++;
    }
    // acumulator = (16 * ticks / TICKS_PER_TURN) + 16 * multiplier;
    acumulator = 16 * (ticks / TICKS_PER_TURN + multiplier);
    acumulator = acumulator >= 360 ? acumulator - 360 : acumulator;
    console.log(i, ticks, multiplier, acumulator);
  }
}

interrupt();