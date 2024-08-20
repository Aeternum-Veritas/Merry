#include "codegen.hpp"

void masm::CodeGen::setup_codegen(SymbolTable *_t, std::vector<Node> *_n)
{
    nodes = _n;
    table = _t;
}

/**
 * A very tough choice:
 * Either I add the logic for every instruction in the same function or I make separate functions that will do the same for a group of similar instruction.
 * The problems: The first approach will offend pro C++ devs or any programmer in general(Even I would be pissed looking at a function that is over a 1000 lines long)
 * There will be an insane amount of redundant code but on the +ve side, things will be very easy and simple.
 * The second approach really has just one problem, it is complicated to implement the specific functions for each group.
 * I have to make the grouping even simpler i.e one group may have just one or two things in common at most which leaves us with at most 2 instructions per group.
 * That isn't a good enough reason to go through writing complex function logic.
 * Thus there will be a hybrid approach. The groups that will help reduce the function size will have a separate function and no otherwise.
 * Even with that, the function should get well over 600 lines of code.
 */

void masm::CodeGen::handle_arithmetic_reg_imm(msize_t op, NodeArithmetic *a)
{
    GenBinary b;
    b.bytes.b1 = op; // the opcode
    // 0xFFFFFFFF
    b.bytes.b2 = a->reg;
    b.full |= std::stoull(std::get<std::string>(a->second_oper));
    code.push_back(b);
}

void masm::CodeGen::handle_arithmetic_reg_reg(msize_t op, NodeArithmetic *a)
{
    GenBinary b;
    b.bytes.b1 = op;
    b.bytes.b8 = a->reg;
    b.bytes.b8 <<= 4;
    b.bytes.b8 |= std::get<Register>(a->second_oper);
    code.push_back(b);
}

void masm::CodeGen::handle_arithmetic_reg_var(NodeArithmetic *a, msize_t op)
{
    GenBinary b;
    std::string var = std::get<std::string>(a->second_oper);
    size_t var_addr = data_addr[std::get<std::string>(a->second_oper)];
    auto var_dets = table->variables[table->_var_list.find(var)->second];
    b.bytes.b8 = a->reg;
    switch (var_dets.type)
    {
    case BYTE:
        b.bytes.b1 = op;
        break;
    case WORD:
        b.bytes.b1 = (op + 1);
        break;
    case DWORD:
        b.bytes.b1 = (op + 2);
        break;
    case QWORD:
        b.bytes.b1 = (op + 3);
        break;
    }
    b.full += var_addr & 0xFFFFFFFFFFFF;
    code.push_back(b);
}

bool masm::CodeGen::generate()
{
    // first generate for the data
    generate_data();
    give_address_to_labels();
    for (auto &node : *nodes)
    {
        switch (node.kind)
        {
        case NOP:
        {
            GenBinary b;
            b.full = 0;
            code.push_back(b);
            break;
        }
        case HLT:
        {
            GenBinary b;
            b.bytes.b1 = OP_HALT;
            code.push_back(b);
            break;
        }
        case ADD_IMM:
            handle_arithmetic_reg_imm(OP_ADD_IMM, (NodeArithmetic *)node.node.get());
            break;
        case SUB_IMM:
            handle_arithmetic_reg_imm(OP_SUB_IMM, (NodeArithmetic *)node.node.get());
            break;
        case MUL_IMM:
            handle_arithmetic_reg_imm(OP_MUL_IMM, (NodeArithmetic *)node.node.get());
            break;
        case DIV_IMM:
            handle_arithmetic_reg_imm(OP_DIV_IMM, (NodeArithmetic *)node.node.get());
            break;
        case MOD_IMM:
            handle_arithmetic_reg_imm(OP_MOD_IMM, (NodeArithmetic *)node.node.get());
            break;
        case ADD_REG:
            handle_arithmetic_reg_reg(OP_ADD_REG, (NodeArithmetic *)node.node.get());
            break;
        case SUB_REG:
            handle_arithmetic_reg_reg(OP_SUB_REG, (NodeArithmetic *)node.node.get());
            break;
        case MUL_REG:
            handle_arithmetic_reg_reg(OP_MUL_REG, (NodeArithmetic *)node.node.get());
            break;
        case DIV_REG:
            handle_arithmetic_reg_reg(OP_DIV_REG, (NodeArithmetic *)node.node.get());
            break;
        case MOD_REG:
            handle_arithmetic_reg_reg(OP_MOD_REG, (NodeArithmetic *)node.node.get());
            break;
        case ADD_MEM:
            handle_arithmetic_reg_var((NodeArithmetic *)node.node.get(), OP_ADD_MEMB);
            break;
        case SUB_MEM:
            handle_arithmetic_reg_var((NodeArithmetic *)node.node.get(), OP_SUB_MEMB);
            break;
        case MUL_MEM:
            handle_arithmetic_reg_var((NodeArithmetic *)node.node.get(), OP_MUL_MEMB);
            break;
        case DIV_MEM:
            handle_arithmetic_reg_var((NodeArithmetic *)node.node.get(), OP_DIV_MEMB);
            break;
        case MOD_MEM:
            handle_arithmetic_reg_var((NodeArithmetic *)node.node.get(), OP_MOD_MEMB);
            break;
        }
    }
    for (auto b : code)
    {
        printf("%lX\n", b.full);
    }
    printf("\n");
    for (auto b : data)
    {
        std::cout << ((int)b)<< "\n";
    }
    printf("\n");
    for (auto l : label_addr)
    {
        printf("%s: %lX\n", l.first, l.second);
    }
    printf("\n");
    for (auto l : data_addr)
    {
        printf("%s: %lX\n", l.first, l.second);
    }
    return true;
}

void masm::CodeGen::generate_data()
{
    size_t start = 0;
    for (auto var : table->variables)
    {
        switch (var.type)
        {
        case BYTE:
        {
            data_addr[var.name] = start;
            data.push_back(std::stoull(var.value) & 0xFF);
            start++;
            break;
        }
        case WORD:
        {
            data_addr[var.name] = start;
            data.push_back(std::stoull(var.value) & 0xFFFF);
            start += 2;
            break;
        }
        case DWORD:
        {
            data_addr[var.name] = start;
            data.push_back(std::stoull(var.value) & 0xFFFFFFFF);
            start += 4;
            break;
        }
        case QWORD:
        {
            data_addr[var.name] = start;
            data.push_back(std::stoull(var.value));
            start += 8;
            break;
        }
        case STRING:
        {
            data_addr[var.name] = start;
            for (auto _c : var.value)
            {
                data.push_back(_c);
            }
            start += var.value.length();
            break;
        }
        case RESB:
        {
            data_addr[var.name] = start;
            size_t len = std::stoull(var.value);
            for (size_t i = 0; i < len; i++)
                data.push_back(0);
            start += len;
            break;
        }
        case RESW:
        {
            data_addr[var.name] = start;
            size_t len = std::stoull(var.value) * 2;
            for (size_t i = 0; i < len; i++)
                data.push_back(0);
            start += len;
            break;
        }
        case RESD:
        {
            data_addr[var.name] = start;
            size_t len = std::stoull(var.value) * 4;
            for (size_t i = 0; i < len; i++)
                data.push_back(0);
            start += len;
            break;
        }
        case RESQ:
        {
            data_addr[var.name] = start;
            size_t len = std::stoull(var.value) * 8;
            for (size_t i = 0; i < len; i++)
                data.push_back(0);
            start += len;
            break;
        }
        }
    }
}

void masm::CodeGen::give_address_to_labels()
{
    size_t i = 0;
    for (auto &l : *nodes)
    {
        switch (l.kind)
        {
        case LABEL:
        {
            NodeName *n = (NodeName *)l.node.get();
            label_addr[n->name] = i;
            i++;
            break;
        }
        default:
            if (l.kind >= NOP)
                i++;
            // for instructions that take two qwords, we need another one here
        }
    }
}