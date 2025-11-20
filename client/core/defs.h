#ifndef DEFS_H
#define DEFS_H

namespace appguard {
enum ViolationType {
    NoViolation = 0,
    HashViolation = 100,
    DebuggerViolation = 200,
    RemoteDebuggerViolation = 201
};
}

#endif // DEFS_H
