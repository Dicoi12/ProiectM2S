/*
 *  Multi2Sim
 *  Copyright (C) 2012  Rafael Ubal (ubal@ece.neu.edu)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "Alu.h"
#include "Timing.h"
#include <unordered_map> // Pentru hash map
#include "GeneticValuePredictor.h"
#include "ValuePredictor.h"
#include <vector>
#include <iomanip>
#include <queue>
namespace x86
{

std::unordered_map<long long, int> result_cache; 
long long reuse_count = 0;
long long total_instructions = 0;
long long trivial_instructions = 0;
long long total_alu_instructions = 0;
long long total_load_instructions = 0;
long long reused_instructions = 0;

struct ReusableInstruction {
	long long id;
	Uinst::Opcode opcode;
	int input1;
	int input2;
};

const int REUSE_TABLE_SIZE = 64;
std::queue<ReusableInstruction> reuse_queue;

GeneticValuePredictor predictor;
ValuePredictor value_predictor;

struct TrivialInstruction {
	long long id;
	Uinst::Opcode opcode;
	int input1;
	int input2;
	std::string description;
};

std::vector<TrivialInstruction> trivial_instructions_list;
std::vector<ReusableInstruction> reusable_instructions_list;

int Alu::configuration[FunctionalUnit::TypeCount][3] =
{
	{ 0, 0, 0 },  // Unused

	{ 3, 1, 1 },  // IntAdd
	{ 1, 3, 1 },  // IntMult
	{ 1, 14, 11 },  // IntDiv

	{ 3, 2, 2 },  // EffAddr
	{ 3, 1, 1 },  // Logic

	{ 1, 2, 1 },  // FloatSimple
	{ 1, 3, 1 },  // FloatAdd
	{ 1, 3, 1 },  // FloatCompare
	{ 1, 5, 1 },  // FloatMult
	{ 1, 12, 5 },  // FloatDiv
	{ 1, 22, 14 },  // FloatComplex

	{ 1, 1, 1 },  // XmmIntAdd
	{ 1, 3, 1 },  // XmmIntMult
	{ 1, 14, 11 },  // XmmIntDiv

	{ 1, 1, 1 },  // XmmLogic

	{ 1, 3, 1 },  // XmmFloatAdd
	{ 1, 3, 1 },  // XmmFloatComeare
	{ 1, 5, 1 },  // XmmFloatMult
	{ 1, 12, 6 },  // XmmFloatDiv
	{ 1, 3, 1 },  // XmmFloatConv
	{ 1, 22, 14 }  // XmmFloatComplex
};

const FunctionalUnit::Type Alu::type_table[Uinst::OpcodeCount] =
{
	FunctionalUnit::TypeNone,  // UInstNop

	FunctionalUnit::TypeNone,  // UInstMove
	FunctionalUnit::TypeIntAdd,  // UInstAdd
	FunctionalUnit::TypeIntAdd,  // UInstSub
	FunctionalUnit::TypeIntMult,  // UInstMult
	FunctionalUnit::TypeIntDiv,  // UInstDiv
	FunctionalUnit::TypeEffAddr,  // UInstEffAddr

	FunctionalUnit::TypeLogic,  // UInstAnd
	FunctionalUnit::TypeLogic,  // UInstOr
	FunctionalUnit::TypeLogic,  // UInstXor
	FunctionalUnit::TypeLogic,  // UInstNot
	FunctionalUnit::TypeLogic,  // UInstShift
	FunctionalUnit::TypeLogic,  // UInstSign

	FunctionalUnit::TypeNone,  // UInstFpMove
	FunctionalUnit::TypeFloatSimple,  // UInstFpSign
	FunctionalUnit::TypeFloatSimple,  // UInstFpRound

	FunctionalUnit::TypeFloatAdd,  // UInstFpAdd
	FunctionalUnit::TypeFloatAdd,  // UInstFpSub
	FunctionalUnit::TypeFloatCompare,  // UInstFpComp
	FunctionalUnit::TypeFloatMult,  // UInstFpMult
	FunctionalUnit::TypeFloatDiv,  // UInstFpDiv

	FunctionalUnit::TypeFloatComplex,  // UInstFpExp
	FunctionalUnit::TypeFloatComplex,  // UInstFpLog
	FunctionalUnit::TypeFloatComplex,  // UInstFpSin
	FunctionalUnit::TypeFloatComplex,  // UInstFpCos
	FunctionalUnit::TypeFloatComplex,  // UInstFpSincos
	FunctionalUnit::TypeFloatComplex,  // UInstFpTan
	FunctionalUnit::TypeFloatComplex,  // UInstFpAtan
	FunctionalUnit::TypeFloatComplex,  // UInstFpSqrt
};


void Alu::ParseConfiguration(misc::IniFile *ini_file)
{
	// Section
	std::string section = "FunctionalUnits";

	// Get configuration parameter
	for (int i = 1; i < FunctionalUnit::TypeCount; i++)
	{
		// Functional unit name
		std::string name = FunctionalUnit::type_map[i];
		assert(!name.empty());

		// Number of instances
		configuration[i][0] = ini_file->ReadInt(section,
				name + ".Count",
				configuration[i][0]);

		// Get total operation latency
		configuration[i][1] = ini_file->ReadInt(section,
				name + ".OpLat",
				configuration[i][1]);

		// Get issue latency
		configuration[i][2] = ini_file->ReadInt(section,
				name + ".IssueLat",
				configuration[i][2]);
	}
}


void Alu::DumpConfiguration(std::ostream &os)
{
	// Title
	os << "[ Config.FunctionalUnits ]\n";

	// Traverse functional units
	for (int i = 1; i < FunctionalUnit::TypeCount; i++)
	{
		// Functional unit name
		std::string name = FunctionalUnit::type_map[i];
		assert(!name.empty());

		// Print configuration
		os << misc::fmt("%s.Count = %d\n", name.c_str(), configuration[i][0]);
		os << misc::fmt("%s.OpLat = %d\n", name.c_str(), configuration[i][1]);
		os << misc::fmt("%s.IssueLat = %d\n", name.c_str(), configuration[i][2]);
	}

	// Done
	os << '\n';
}


Alu::Alu()
{
	// Reserve functional unit vector entries
	functional_units.resize(FunctionalUnit::TypeCount);

	// Create functional units
	for (int type = 1; type < FunctionalUnit::TypeCount; type++)
		functional_units[type] = misc::new_unique<FunctionalUnit>(
				(FunctionalUnit::Type) type,
				FunctionalUnit::type_map[type],
				configuration[type][0],
				configuration[type][1],
				configuration[type][2]);
}

bool isTrivialInstruction(Uop *uop) {
	auto type = uop->getUinst()->getOpcode();
	bool is_trivial = false;
	std::string description;
	
	switch (type) {
		case Uinst::OpcodeAdd:
			if (uop->getInput(1) == 0) {
				is_trivial = true;
				description = "x + 0";
			}
			break;
			
		case Uinst::OpcodeSub:
			if (uop->getInput(1) == 0) {
				is_trivial = true;
				description = "x - 0";
			} else if (uop->getInput(0) == uop->getInput(1)) {
				is_trivial = true;
				description = "x - x";
			}
			break;
			
		case Uinst::OpcodeMult:
			if (uop->getInput(1) == 0) {
				is_trivial = true;
				description = "x * 0";
			} else if (uop->getInput(1) == 1) {
				is_trivial = true;
				description = "x * 1";
			}
			break;
			
		case Uinst::OpcodeDiv:
			if (uop->getInput(0) == 0) {
				is_trivial = true;
				description = "0 / x";
			} else if (uop->getInput(0) == uop->getInput(1)) {
				is_trivial = true;
				description = "x / x";
			} else if (uop->getInput(1) == 1) {
				is_trivial = true;
				description = "x / 1";
			}
			break;
			
		default:
			return false;
	}
	
	if (is_trivial) {
		TrivialInstruction ti;
		ti.id = uop->getId();
		ti.opcode = type;
		ti.input1 = uop->getInput(0);
		ti.input2 = uop->getInput(1);
		ti.description = description;
		trivial_instructions_list.push_back(ti);
	}
	
	return is_trivial;
}

bool isReusableInstruction(Uop *uop) {
	auto type = uop->getUinst()->getOpcode();
	int op1 = uop->getInput(0);
	int op2 = uop->getInput(1);
	
	bool found = false;
	size_t queue_size = reuse_queue.size();
	for (size_t i = 0; i < queue_size; i++) {
		const ReusableInstruction& ri = reuse_queue.front();
		if (ri.opcode == type && ri.input1 == op1 && ri.input2 == op2) {
			found = true;
		}
		reuse_queue.push(reuse_queue.front());
		reuse_queue.pop();
	}
	
	if (found) {
		return true;
	}
	
	ReusableInstruction new_entry;
	new_entry.id = uop->getId();
	new_entry.opcode = type;
	new_entry.input1 = op1;
	new_entry.input2 = op2;
	
	if (reuse_queue.size() >= REUSE_TABLE_SIZE) {
		reuse_queue.pop();
	}
	
	reuse_queue.push(new_entry);
	
	return false;
}

int Alu::Reserve(Uop *uop)
{
	// Current cycle
	Timing *timing = Timing::getInstance();
	long long cycle = timing->getCycle();

	// Record the first attempt of the uop to reserve a functional unit
	if (!uop->first_alu_cycle)
		uop->first_alu_cycle = cycle;

	// Get the functional unit type required by the uop
	FunctionalUnit::Type type = type_table[uop->getUinst()->getOpcode()];
	if (type == FunctionalUnit::TypeNone)
		return 1;

	total_instructions++;

	if (isTrivialInstruction(uop)) {
		trivial_instructions++;
	}

	if (isReusableInstruction(uop)) {
		reused_instructions++;
	}

	long long uop_id = uop->getId();
	

	assert(type > FunctionalUnit::TypeNone && type < FunctionalUnit::TypeCount);
	FunctionalUnit *functional_unit = functional_units[type].get();
	assert(functional_unit);

	long long genetic_predicted_value;
	long long value_predicted_value;
	bool genetic_prediction = predictor.predict(uop, genetic_predicted_value);
	bool value_prediction = value_predictor.predict(uop, value_predicted_value);

	if (genetic_prediction && value_prediction) {
		if (predictor.isConfidentPrediction(uop) && !value_predictor.isConfidentPrediction(uop)) {
		} else if (!predictor.isConfidentPrediction(uop) && value_predictor.isConfidentPrediction(uop)) {

		}
	} else if (genetic_prediction) {
	} else if (value_prediction) {
	}

	int latency = functional_unit->Reserve(uop);
	result_cache[uop_id] = latency;

	predictor.update(uop, latency);
	value_predictor.update(uop, latency);

	return latency;
}


void Alu::ReleaseAll()
{
	for (int type = 1; type < FunctionalUnit::TypeCount; type++)
		functional_units[type]->Release();
}


void Alu::DumpTrivialInstructions(std::ostream &os) const {
	os << "\nTrivial Instructions Table:\n";
	os << std::string(80, '-') << "\n";
	os << std::setw(10) << "ID" << " | "
	   << std::setw(15) << "Opcode" << " | "
	   << std::setw(10) << "Input1" << " | "
	   << std::setw(10) << "Input2" << " | "
	   << std::setw(20) << "Description" << "\n";
	os << std::string(80, '-') << "\n";
	
	for (const auto &ti : trivial_instructions_list) {
		os << std::setw(10) << ti.id << " | "
		   << std::setw(15) << Uinst::getInfo(ti.opcode)->name << " | "
		   << std::setw(10) << ti.input1 << " | "
		   << std::setw(10) << ti.input2 << " | "
		   << std::setw(20) << ti.description << "\n";
	}
	os << std::string(80, '-') << "\n";
}

void Alu::DumpReport(std::ostream &os) const
{
	// Header
	os << "; Functional unit pool\n";
	os << ";    Accesses - Number of uops issued to a f.u.\n";
	os << ";    Denied - Number of requests denied due to busy f.u.\n";
	os << ";    WaitingTime - Average number of waiting cycles to reserve f.u.\n";

	// Traverse functional units
	for (int type = 1; type < FunctionalUnit::TypeCount; type++)
	{
		FunctionalUnit *functional_unit = functional_units[type].get();
		os << misc::fmt("fu.%s.Accesses = %lld\n",
				functional_unit->getName().c_str(),
				functional_unit->getNumAccesses());
		os << misc::fmt("fu.%s.Denied = %lld\n",
				functional_unit->getName().c_str(),
				functional_unit->getNumDeniedAccesses());
		os << misc::fmt("fu.%s.WaitingTime = %.4g\n",
				functional_unit->getName().c_str(),
				functional_unit->getNumAccesses() ?
				(double) functional_unit->getWaitingTime()
				/ functional_unit->getNumAccesses()
				: 0.0);
	}

	os << misc::fmt("Total Instructions = %lld\n", total_instructions);
	os << misc::fmt("Total ALU Instructions = %lld\n", total_alu_instructions);

	os << misc::fmt("Total Load Instructions = %lld\n", total_load_instructions);

	os << misc::fmt("Reused Instructions = %lld\n", reused_instructions);
	os << misc::fmt("Reuse Percentage = %.2f%%\n",
			total_instructions ? (double)reused_instructions / total_instructions * 100 : 0.0);

	os << misc::fmt("Trivial Instructions = %lld\n", trivial_instructions);
	os << misc::fmt("Trivial Percentage = %.2f%%\n",
			total_instructions ? (double)trivial_instructions / total_instructions * 100 : 0.0);

	DumpTrivialInstructions(os);

	os << "\nGenetic Value Predictor Statistics:\n";
	predictor.dumpStats(os);
	
	os << "\nValue Predictor Statistics:\n";
	value_predictor.dumpStats(os);

	os << misc::fmt("Reuse Queue Size = %d\n", REUSE_TABLE_SIZE);
	os << misc::fmt("Reuse Queue Current Size = %d\n", reuse_queue.size());

	// Done
	os << '\n';
}


}

