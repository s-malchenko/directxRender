#pragma once

#include "Technique.h"
#include <string>
#include <vector>

namespace TechniqueFactory
{
	const std::vector<std::string>& GetAvailableTechniques();
	Technique CreateTechnique(const std::string& name);
	Technique CreateDefaultTechnique();
}