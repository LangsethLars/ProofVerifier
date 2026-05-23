#pragma once

#include "PropExpr.h"
#include "IntExpr.h"
#include <string>
#include <unordered_map>

// A proof assumption: a named hypothesis with a propositional type.
// Example:  h : Prop.And(p q)
struct ProofAssumption {
	std::string name;
	PropExprPtr type;
};

// The proof-checker state that is threaded through each ProofStep.
// It is a value type so that CasesStep can snapshot and branch cheaply.
struct ProofContext {
	// The proposition that still needs to be proved.
	PropExprPtr goal;

	// Proof assumptions introduced by AssumeStep / HaveStep.
	// Maps hypothesis name -> assumption.
	std::unordered_map<std::string, ProofAssumption> assumptions;

	// Convenience: add a new hypothesis, throws if name already exists.
	void addAssumption(const std::string& name, PropExprPtr type) {
		if (assumptions.count(name)) {
			throw std::runtime_error("Duplicate hypothesis name: " + name);
		}
		assumptions[name] = ProofAssumption{ name, type };
	}
};
