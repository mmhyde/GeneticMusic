#pragma once

#include "Fitness/FitnessEvaluator.h"
#include "Selection/Selector.h"
#include "Breeding/Breeder.h"
#include "Mutation/Mutator.h"

namespace Genetics {

	using FitnessType   = FitnessEvaluator<AutomaticFitness>;
	using SelectionType = Selection<TournamentSelection>;
    using BreederType   = BreedingMethod<InterpolateBreed>;
//  using MutationType  = Mutation<>;
	using PruningType   = ElitistPrune;

} // namespace Genetics