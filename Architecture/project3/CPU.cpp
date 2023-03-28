/******************************
 * Submitted by: Alex Agruso
 * CS 3339 - Spring 2023, Texas State University
 * Project 2 Emulator
 * Copyright 2021, Lee B. Hinkle, all rights reserved
 * Based on prior work by Martin Burtscher and Molly O'Neil
 * Redistribution in source or binary form, with or without modification,
 * is *not* permitted. Use in source or binary form, with or without
 * modification, is only permitted for academic use in CS 3339 at
 * Texas State University.
 ******************************/

#include "CPU.h"
#include "ALU.h"
#include "Stats.h"

#include <cstdint>
#include <iomanip>

const string CPU::regNames[] = {"$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
                                "$t0",   "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
                                "$s0",   "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
                                "$t8",   "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"};

CPU::CPU(uint32_t pc, Memory &iMem, Memory &dMem) : pc(pc), iMem(iMem), dMem(dMem)
{
    for (int i = 0; i < NREGS; i++)
    {
        regFile[i] = 0;
    }

    hi = 0;
    lo = 0;
    regFile[28] = 0x10008000;                  // gp
    regFile[29] = 0x10000000 + dMem.getSize(); // sp

    instructions = 0;
    stop = false;
}

void CPU::run()
{
    while (!stop)
    {
        instructions++;

        fetch();
        decode();
        execute();
        mem();
        writeback();

        D(printRegFile());
    }
}

void CPU::fetch()
{
    instr = iMem.loadWord(pc);
    pc = pc + 4;
}

/////////////////////////////////////////
// ALL YOUR CHANGES GO IN THIS FUNCTION
/////////////////////////////////////////
void CPU::decode()
{
    uint32_t opcode;     // opcode field
    uint32_t rs, rt, rd; // register specifiers
    uint32_t shamt;      // shift amount (R-type)
    uint32_t funct;      // funct field (R-type)
    uint32_t uimm;       // unsigned version of immediate (I-type)
    int32_t simm;        // signed version of immediate (I-type)
    uint32_t addr;       // jump address offset field (J-type)

    opcode = (instr >> 26) & 0x3F;
    rs = (instr >> 21) & 0x1F;
    rt = (instr >> 16) & 0x1F;
    rd = (instr >> 11) & 0x1F;
    shamt = (instr >> 6) & 0x1F;
    funct = instr & 0x3F;
    uimm = instr & 0xFFFF;
    simm = (int32_t)uimm << 16 >> 16;
    addr = instr & 0x03FFFFFF;

    // Hint: you probably want to give all the control signals some "safe"
    // default value here, and then override their values as necessary in
    // each case statement below!
    writeDest = false;
    destReg = 0;
    aluOp = ADD;
    aluSrc1 = 0;
    aluSrc2 = 0;
    opIsMultDiv = false;
    opIsLoad = false;
    opIsStore = false;

    D(cout << "  " << hex << setw(8) << pc - 4 << ": ");
    switch (opcode)
    {
    case 0x00:
        switch (funct)
        {
        case 0x00:
            D(cout << "sll " << regNames[rd] << ", " << regNames[rs] << ", " << dec << shamt);
            writeDest = true;
            stats.registerDest(rd);
            destReg = rd;
            aluOp = SHF_L;
            aluSrc1 = regFile[rs];
            aluSrc2 = shamt;
            stats.registerSrc(rs);
            stats.clock();
            break; // use prototype above, not the greensheet
        case 0x03:
            D(cout << "sra " << regNames[rd] << ", " << regNames[rs] << ", " << dec << shamt);
            writeDest = true;
            stats.registerDest(rd);
            destReg = rd;
            aluOp = SHF_R;
            aluSrc1 = regFile[rs];
            aluSrc2 = shamt;
            stats.registerSrc(rs);
            stats.clock();
            break; // use prototype above, not the greensheet
        case 0x08:
            D(cout << "jr " << regNames[rs]);
            pc = regFile[rs];
            stats.registerSrc(rs);
            stats.clock();
            break;
        case 0x10:
            D(cout << "mfhi " << regNames[rd]);
            stats.registerDest(rd);
            writeDest = true;
            destReg = rd;
            aluOp = ADD;
            aluSrc1 = hi;
            aluSrc2 = 0;
            stats.registerSrc(hi);
            stats.clock();
            break;
        case 0x12:
            D(cout << "mflo " << regNames[rd]);
            stats.registerDest(rd);
            writeDest = true;
            destReg = rd;
            aluOp = ADD;
            aluSrc1 = lo;
            aluSrc2 = 0;
            stats.registerSrc(lo);
            stats.clock();
            break;
        case 0x18:
            D(cout << "mult " << regNames[rs] << ", " << regNames[rt]);
            stats.registerDest(REG_HILO);
            opIsMultDiv = true;
            aluOp = MUL;
            aluSrc1 = regFile[rs];
            aluSrc2 = regFile[rt];
            stats.registerSrc(rs);
            stats.registerSrc(rt);
            stats.clock();
            break;
        case 0x1a:
            D(cout << "div " << regNames[rs] << ", " << regNames[rt]);
            stats.registerDest(REG_HILO);
            opIsMultDiv = true;
            aluOp = DIV;
            aluSrc1 = regFile[rs];
            aluSrc2 = regFile[rt];
            stats.registerSrc(rs);
            stats.registerSrc(rt);
            stats.clock();
            break;
        case 0x21:
            D(cout << "addu " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt]);
            stats.registerDest(rd);
            writeDest = true;
            destReg = rd;
            aluOp = ADD;
            aluSrc1 = regFile[rs];
            aluSrc2 = regFile[rt];
            stats.registerSrc(rs);
            stats.registerSrc(rt);
            stats.clock();
            break;
        case 0x23:
            D(cout << "subu " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt]);
            stats.registerDest(rd);
            writeDest = true;
            destReg = rd;
            aluOp = ADD;
            aluSrc1 = regFile[rs];
            aluSrc2 = -regFile[rt];
            stats.registerSrc(rs);
            stats.registerSrc(rt);
            stats.clock();
            break; // hint: subtract is the same as adding a negative
        case 0x2a:
            D(cout << "slt " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt]);
            stats.registerDest(rd);
            writeDest = true;
            destReg = rd;
            aluOp = CMP_LT;
            aluSrc1 = regFile[rs];
            aluSrc2 = regFile[rt];
            stats.registerSrc(rs);
            stats.registerSrc(rt);
            stats.clock();
            break;
        default:
            cerr << "unimplemented instruction: pc = 0x" << hex << pc - 4 << endl;
        }
        break;
    case 0x02:
        D(cout << "j " << hex << ((pc & 0xf0000000) | addr << 2)); // P1: pc + 4
        pc = (pc & 0xF0000000) | (addr << 2);
        break;
    case 0x03:
        D(cout << "jal " << hex << ((pc & 0xf0000000) | addr << 2)); // P1: pc + 4
        stats.registerDest(REG_RA);
        writeDest = true;
        destReg = REG_RA; // writes PC+4 to $ra
        aluOp = ADD;      // pc needs to pass through the ALU unchanged
        aluSrc1 = pc;
        aluSrc2 = regFile[REG_ZERO]; // always reads zero
        pc = (pc & 0xf0000000) | addr << 2;
        stats.clock();
        break;
    case 0x04:
        D(cout << "beq " << regNames[rs] << ", " << regNames[rt] << ", " << pc + (simm << 2));
        if (regFile[rs] == regFile[rt])
        {
            pc += (simm << 2);
            stats.flush(ID - IF1);
            stats.countTaken();
        }
        stats.registerSrc(rs);
        stats.registerSrc(rt);
        stats.countBranch();
        stats.clock();
        break; // read the handout carefully, update PC directly here as in jal
               // example
    case 0x05:
        D(cout << "bne " << regNames[rs] << ", " << regNames[rt] << ", " << pc + (simm << 2));
        if (regFile[rs] != regFile[rt])
        {
            pc += (simm << 2);
            stats.flush(ID - IF1);
            stats.countTaken();
        }
        stats.registerSrc(rs);
        stats.registerSrc(rt);
        stats.countBranch();
        stats.clock();
        break; // same comment as beq
    case 0x09:
        D(cout << "addiu " << regNames[rt] << ", " << regNames[rs] << ", " << dec << simm);
        writeDest = true;
        stats.registerDest(rt);
        destReg = rt;
        aluOp = ADD;
        aluSrc1 = regFile[rs];
        aluSrc2 = simm;
        stats.registerSrc(rs);
        stats.clock();
        break;
    case 0x0c:
        D(cout << "andi " << regNames[rt] << ", " << regNames[rs] << ", " << dec << uimm);
        stats.registerDest(rt);
        writeDest = true;
        destReg = rt;
        aluOp = AND;
        aluSrc1 = regFile[rs];
        aluSrc2 = uimm;
        stats.registerSrc(rs);
        stats.clock();
        break;
    case 0x0f:
        D(cout << "lui " << regNames[rt] << ", " << dec << simm);
        stats.registerDest(rt);
        writeDest = true;
        destReg = rt;
        aluOp = ADD;
        aluSrc1 = (simm << 16);
        aluSrc2 = 0;
        stats.clock();
        break; // use the ALU to execute necessary op, you may set aluSrc2 = xx
               // directly
    case 0x1a:
        D(cout << "trap " << hex << addr);
        switch (addr & 0xf)
        {
        case 0x0:
            cout << endl;
            break;
        case 0x1:
            cout << " " << (signed)regFile[rs];
            stats.registerSrc(rs);
            stats.clock();
            break;
        case 0x5:
            cout << endl << "? ";
            cin >> regFile[rt];
            stats.registerDest(rt);
            stats.clock();
            break;
        case 0xa:
            stop = true;
            break;
        default:
            cerr << "unimplemented trap: pc = 0x" << hex << pc - 4 << endl;
            stop = true;
        }
        break;
    case 0x23:
        D(cout << "lw " << regNames[rt] << ", " << dec << simm << "(" << regNames[rs] << ")");
        stats.registerDest(rt);
        opIsLoad = true;
        writeDest = true;
        destReg = rt;
        aluOp = ADD;
        aluSrc1 = regFile[rs];
        aluSrc2 = simm;
        stats.registerSrc(rs);
        stats.countMemOp();
        stats.clock();
        break; // do not interact with memory here - setup control
               // signals for mem()
    case 0x2b:
        D(cout << "sw " << regNames[rt] << ", " << dec << simm << "(" << regNames[rs] << ")");
        opIsStore = true;
        storeData = regFile[rt];
        aluOp = ADD;
        aluSrc1 = regFile[rs];
        aluSrc2 = simm;
        stats.registerSrc(rt);
        stats.registerSrc(rs);
        stats.countMemOp();
        stats.clock();
        break; // same comment as lw
    default:
        cerr << "unimplemented instruction: pc = 0x" << hex << pc - 4 << endl;
    }
    D(cout << endl);
}

void CPU::execute()
{
    aluOut = alu.op(aluOp, aluSrc1, aluSrc2);
}

void CPU::mem()
{
    if (opIsLoad)
        writeData = dMem.loadWord(aluOut);
    else
        writeData = aluOut;

    if (opIsStore)
        dMem.storeWord(storeData, aluOut);
}

void CPU::writeback()
{
    if (writeDest && destReg > 0) // skip when write is to zero_register
        regFile[destReg] = writeData;

    if (opIsMultDiv)
    {
        hi = alu.getUpper();
        lo = alu.getLower();
    }
}

void CPU::printRegFile()
{
    cout << hex;
    for (int i = 0; i < NREGS; i++)
    {
        cout << "    " << regNames[i];
        if (i > 0)
            cout << "  ";
        cout << ": " << setfill('0') << setw(8) << regFile[i];
        if (i == (NREGS - 1) || (i + 1) % 4 == 0)
            cout << endl;
    }
    cout << "    hi   : " << setfill('0') << setw(8) << hi;
    cout << "    lo   : " << setfill('0') << setw(8) << lo;
    cout << dec << endl;
}

void CPU::printFinalStats()
{
    std::cout << "Program finished at pc = 0x" << hex << pc << " (" << dec << instructions
              << " instructions executed)"
              << "\n\n";

    std::cout << fixed << setprecision(2);
    std::cout << "Cycles: " << stats.getCycles() << '\n';
    std::cout << "CPI: " << stats.getCycles() / (float)instructions << "\n\n";

    std::cout << "Bubbles: " << stats.getBubbles() << '\n';
    std::cout << "Flushes: " << stats.getFlushes() << "\n\n";

    std::cout << fixed << setprecision(1);
    std::cout << "Mem ops: " << (stats.getMemOps() / (float)instructions) * 100
              << "% of instructions" << '\n';
    std::cout << "Branches: " << (stats.getBranches() / (float)instructions) * 100
              << "% of instructions" << '\n';
    std::cout << "  % Taken: " << (stats.getTaken() / (float)stats.getBranches()) * 100 << '\n';
}
