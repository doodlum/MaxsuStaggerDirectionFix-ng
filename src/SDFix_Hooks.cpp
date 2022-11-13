#include "SDFix_Hooks.h"

namespace SDFix
{
	bool ResetStaggerDireHook::SetGraphVarFloat(RE::BShkbAnimationGraph* Graph, const RE::BSFixedString& Var_name, float staggerDire)
	{
		static const std::string StaggerDireName = "staggerDirection";
		static const std::string ModifiedDireName = "Maxsu_modifiedStaggerDirection";

		if (Var_name.c_str() == StaggerDireName && staggerDire == 0.f) {
			logger::debug("Reset stagger Direction Float Hook Trigger!");

			if (Graph && Graph->rootNode && Graph->rootNode->GetUserData()) {
				auto target = Graph->rootNode->GetUserData();

				float ModifiedDire;
				if (target->GetGraphVariableFloat(ModifiedDireName, ModifiedDire))
				{
					logger::debug(FMT_STRING("Get Modified Stagger Direction is {}"), ModifiedDire);
					logger::debug(FMT_STRING("Reset a Target Ref! Form ID is {:x}, Name is {}"), target->GetFormID(), target->GetName());
					return _SetGraphVarFloat(Graph, Var_name, ModifiedDire);
				}
				else
					logger::debug("Fail to get Modified Stagger Direction!");

			} else
				logger::debug(FMT_STRING("Can not find a Target Ref!"));
		}
		
		return _SetGraphVarFloat(Graph, Var_name, staggerDire);
	}
}
