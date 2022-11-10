const std = @import("std");
const Feature = std.Target.arm.Feature;
const featureSet = std.Target.Cpu.Feature.feature_set_fns(Feature).featureSet;
const CpuModel = std.Target.Cpu.Model;

pub fn build(b: *std.build.Builder) void {
    const target = .{
        // our Pi A+/Zero uses armv6, which is under the 'arm' umbrella with 
        // the other 32-bit arm architectures.
        .cpu_arch = .arm, 
        // specific CPU model of our pi
        .cpu_model =  .{
            .explicit = &std.Target.arm.cpu.arm1176jzf_s
        },

        // We mark the OS as freestanding since this is a bare metal environment.
        .os_tag = .freestanding,
        // The ABI (Application Binary Interface) defines how different
        // compiled modules communicate with each other, (i.e. if a functions
        // arguments are passed on registers or on the stack.) Here i am using
        // EABI which is the ARM bare metal abi standard. There is also
        // EABIHF which supports hardware float. As I understand that, it only
        // affects the way floats get passed as args (i.e. using registers, vs
        // something else).
        .abi = .eabi,
    };

    // Standard release options allow the person running `zig build` to select
    // between Debug, ReleaseSafe, ReleaseFast, and ReleaseSmall.
    const mode = b.standardReleaseOptions();

    const exe = b.addExecutable("blink.bin", "prog/main.zig");
    // add our start.S file to our executable 
    exe.addAssemblyFile("board/rpi-A-Zero/asm/start.S");
    
    // I dont think this is strictly necessary, but it cant hurt 
    exe.entry_symbol_name = "zigMain";

    // now we apply the target and build mode options we configured above to our exe 
    exe.setTarget(target);
    exe.setBuildMode(mode);

    // Tell compiler to use our linker script
    exe.setLinkerScriptPath(std.build.FileSource{
        .path = "board/rpi-A-Zero/memmap.ld",
    });
    
    // tell builder we want to emit auto documentation
    exe.emit_docs = .emit;

    // now we tell the builder to actually output our executable.
    exe.install(); 






    // This is for troubleshooting, and comes in handy for debugging linker
    // script problems. This step disassembles the intermediate ELF executable
    // This step depends on the install step, since it requires that the ELF
    // file be in 'zig-out/bin/', and the install step of the build process is
    // what places it there. Run with 'zig build dump-elf'
    const dump_elf_cmd = b.addSystemCommand(&[_][]const u8{
        // change this to whatever objdump version you use. I use LLVM 15.0.3 objdump.
        "llvm-objdump",
        "-D",
        b.getInstallPath(.{ .custom = "bin" }, exe.out_filename),
    });
    // make the command depend on the main executable being built.
    dump_elf_cmd.step.dependOn(b.getInstallStep());
    // make the dump elf step for the user to invoke via 'zig build dump-elf'
    const dump_elf_step = b.step("dump-elf", "Disassemble the ELF executable");
    // the compiler step depends on the command we created, which in turn depends on 
    // the ELF executable being built.
    dump_elf_step.dependOn(&dump_elf_cmd.step);
    
    // This is for getting sizes of sections from our executable 
    // Useful for checking if our binaries are needlessly inflated, and by what.
    const dump_sections_cmd = b.addSystemCommand(&[_][]const u8{
        "llvm-objdump",
        "-h",
        b.getInstallPath(.{ .custom = "bin" }, exe.out_filename),
    });
    dump_sections_cmd.step.dependOn(b.getInstallStep());
    const dump_sections_step = b.step("dump-headers", "Dump the executable's section titles and their sizes.");
    dump_sections_step.dependOn(&dump_sections_cmd.step);



    // this step will build and install (with my-install) the binary on our pi.
    const install_cmd = b.addSystemCommand(&[_][]const u8{
        "pi-install",
        "zig-out/bin/blink.bin",
    });
    install_cmd.step.dependOn(b.getInstallStep()); 
    const install_step = b.step("pi-install", "Run the program on the pi with my-install");
    install_step.dependOn(&install_cmd.step);
    
    
    //const exe_tests = b.addTest("src/main.zig");
    //exe_tests.setTarget(target);
    //exe_tests.setBuildMode(mode);

    //const test_step = b.step("test", "Run unit tests");
    //test_step.dependOn(&exe_tests.step);
}
