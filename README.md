# EAE Controls and Firmware Submission
Firmware section 7 and 7.1 submission of epiroc's proEAE Engineering challenge. Developed in an ubuntu 24.04.03 virtual machine compiled in gcc. 

## Compilation

Complie and run the code with the following
```
gcc src/*.c -Iinclude -o run.out
./run.out
```

HOW TO RUN
Initial float temperature of the system can be set in arguments. The runtime of the program can be set with the second argument (in seconds)
```
./run.out 55.0 0.1
```

## Improvements
This implementation was designed to run in a simulated Linux environment, prioritizing functional correctness over real-time determinism. Future iterations could more closely mimic a true bare-metal or RTOS-based system by improving the runtime execution model and fault-handling mechanisms. Specifically:
- Deterministic Execution: Replace the current usleep()-based delay with a periodic scheduler or RTOS task to achieve consistent, time-triggered behavior similar to an embedded control loop.
- Watchdog Integration: Implement a watchdog reset within the main control loop to detect and recover from potential hangs or blocking conditions.
- Robust Fault Handling: Introduce structured error management to handle invalid or out-of-range sensor readings gracefully, preventing undefined behavior.
- Timestamped Logging: Incorporate real-time timestamps in log outputs to support debugging, performance analysis, and traceability during runtime testing.
- This structured approach would enhance the system’s reliability, timing determinism, and maintainability—qualities essential to production-grade embedded firmware.
- PID constants not tuned and cannot be tuned properly without a proper control system anaylsis. The control system is theoretical. I tried to tune it but I needed to implement more features