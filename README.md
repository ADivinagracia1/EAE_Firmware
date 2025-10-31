# EAE Controls and Firmware Submission
Firmware section 7 and 7.1 submission for Epiroc’s EAE Engineering Challenge. Developed on Ubuntu 24.04.03 VM and compiled with GCC.

## Compilation

To compile the firmware:
```
gcc src/*.c -Iinclude -o run.out -lm
```

To run the program:
```
./run.out [initial_temperature] [loop_period_s]
```

Arguments:

initial_temperature – Initial float temperature in °C (default: 20.0, clamped between 20.0 and 100.0).

loop_period_s – Runtime loop period in seconds (default: 1.0, clamped between 0.05 and 5.0).

Example:
```
./run.out 55.0 0.1
```
This starts the program with a 55°C initial temperature and a 100 ms loop period.

## Improvements
This implementation was designed to run in a simulated Linux environment, prioritizing functional correctness over real-time determinism. Future iterations could more closely mimic a true bare-metal or RTOS-based system by improving the runtime execution model and fault-handling mechanisms. Specifically:
- Deterministic Execution: Replace the current usleep()-based delay with a periodic scheduler or RTOS task to achieve consistent, time-triggered behavior similar to an embedded control loop.
- Watchdog Integration: Implement a watchdog reset within the main control loop to detect and recover from potential hangs or blocking conditions.
- Robust Fault Handling: Introduce structured error management to handle invalid or out-of-range sensor readings gracefully, preventing undefined behavior.
- Timestamped Logging: Incorporate real-time timestamps in log outputs to support debugging, performance analysis, and traceability during runtime testing.
- PID Tuning: PID constants are theoretical and not fully tuned; proper tuning requires a complete control system analysis.

This structured approach would enhance the system’s reliability, timing determinism, and maintainability—qualities essential for production-grade embedded firmware.