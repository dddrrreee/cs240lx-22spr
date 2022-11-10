const std = @import("std");

pub fn build(b: *std.build.Builder) !void {
    const exe = b.addExecutable("pi-zig", "src/main.zig");
    exe.addAssemblyFile("src/asm/start.S");

    exe.addIncludePath("libpi/include");
    exe.addIncludePath("libpi/libc");
    exe.addObjectFile("libpi/lib/libpi.a");

    exe.entry_symbol_name = "zigMain";
    // Configure the target. The target tells the compiler details about the
    // hardware and software that is going to run the program. Since we are
    // building a kernel for the RockPro64, we need to use a non-native
    // target for armv8a, freestanding.
    const target = .{
        // The Pine RockPro64 has an ARM CPU (actually 6), so we can set the
        // cpu arch to aarch64 (or arm for 32bit mode)
        .cpu_arch = .arm,
        
        // This is a bit more advanced. Since I know what board im targeting,
        // I also know tvhe exact CPU (Cortex A53 is our boot-core, but we also
        // have two Cortex A72 cores. Since they are both using the same arch
        // version (8.0A, as of writing we are up to 8.5A), we should be fine
        // just selecting one of them to let the compiler know which assembly
        // it is allowed to use.
        .cpu_model = .{ .explicit = &std.Target.arm.cpu.arm1176jzf_s },

        // we mark the os as freestanding since this is a bare metal executable
        .os_tag = .freestanding,

        // The ABI (Application Binary Interface) defines how different
        // compiled modules communicate with each other, (i.e. if a functions
        // arguments are passed on registers or on the stack.) Here i am using
        // EABI which is the ARM bare metal abi standard. There is also
        // EABIHF which supports hardware float. As I understand that, it only
        // affects the way floats get passed as args (i.e. using registers, vs
        // something else). IDK if the RockPro64 has HF, so for now i just wont
        // bother with it.
        .abi = .eabi,
    };

    // Standard release options allow the person running `zig build` to select
    // between Debug, ReleaseSafe, ReleaseFast, and ReleaseSmall.
    const mode = b.standardReleaseOptions();

    exe.setTarget(target);
    
    // I use a custom linker script to setup my binary images. This is b/c the
    // RockPro64/RK3399 expect the first bit of user defined code to be at a
    // specific address, so i need to make sure that our program's entry point
    // is at that address. I also use this to discard several sections of the
    // generated ELF fie that the compiler adds.
    exe.setLinkerScriptPath(std.build.FileSource{
        .path = "pi.ld",
    });

    exe.setBuildMode(mode);
    exe.emit_docs = .emit;
    exe.install();

    // This tells the build system (via 'addInstallRaw') that I want to
    // generate a raw binary image from the ELF executable we generated above.
    // This is the binary our RockPro64 can run. I make this step a dependency
    // of the default 'install step' of the build system, so it will get
    // executed each time you run 'zig build'
    const bin = b.addInstallRaw(exe, "pi-zig.bin", .{});
    b.getInstallStep().dependOn(&bin.step);

    // This is for troubleshooting, and comes in handy for debugging linker
    // script problems. This step disassembles the intermediate ELF executable
    // This step depends on the install step, since it requires that the ELF
    // file be in 'zig-out/bin/', and the install step of the build process is
    // what places it there. Run with 'zig build dump-elf'
    const dumpElfCommand = b.addSystemCommand(&[_][]const u8{
        "arm-none-elf-objdump",
        "-D",
        "-m",
        "arm",
        b.getInstallPath(.{ .custom = "bin" }, exe.out_filename),
    });
    dumpElfCommand.step.dependOn(b.getInstallStep());
    const dumpELFStep = b.step("dump-elf", "Disassemble the ELF executable");
    dumpELFStep.dependOn(&dumpElfCommand.step);

    // As above but for the final binary that will be run on our RockPro64.
    // Run with 'zig build dump-bin'
    const dumpBinCommand = b.addSystemCommand(&[_][]const u8{
        "aarch64-none-elf-objdump",
        "-D",
        "-m",
        "aarch64",
        "-b",
        "binary",
        b.getInstallPath(bin.dest_dir, bin.dest_filename),
    });
    dumpBinCommand.step.dependOn(&bin.step);
    const dumpBinStep = b.step("dump-bin", "Disassemble the raw binary image");
    dumpBinStep.dependOn(&dumpBinCommand.step);

    //Build step to generate docs:
    docs.setBuildMode(mode);
    docs.emit_docs = .emit;
    docs.emit_bin = .no_emit;

    const docs_step = b.step("docs", "Generate docs");
    docs_step.dependOn(&docs.step);
}
