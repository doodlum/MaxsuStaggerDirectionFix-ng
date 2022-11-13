#include "SDFix_Events.h"


namespace SDFix
{
	bool EffectStartEventHandler::RegisterEffectStartEvent()
	{
		static EffectStartEventHandler g_effectstarthandler;

		auto ScriptEventSource = RE::ScriptEventSourceHolder::GetSingleton();

		if (!ScriptEventSource) {
			logger::error("ScriptEventSource not found!");
			return false;
		}

		ScriptEventSource->AddEventSink(&g_effectstarthandler);

		logger::info("Register Effect Start Event Handler!");

		return true;
	}


	EventResult EffectStartEventHandler::ProcessEvent(const RE::TESMagicEffectApplyEvent* a_event, RE::BSTEventSource<RE::TESMagicEffectApplyEvent>* a_eventSource)
	{
		if (!a_event || !a_eventSource) {
			logger::error("Event Source Not Found!");
			return EventResult::kContinue;
		}

		//----------------------Check Hit Causer-----------------------------------------------

		auto hit_causer = a_event->caster.get();

		if (!hit_causer) {
			logger::error("Hit Causer Not Found!");
			return EventResult::kContinue;
		}

		if (!hit_causer->Is3DLoaded()) {
			logger::debug("Hit Causer Not 3D Loaded!");
			return EventResult::kContinue;
		}
		//-------------------------------------------------------------------------------------


		//----------------------Check Hit Target-----------------------------------------------

		auto hit_target = a_event->target.get();

		if (!hit_target) {
			logger::error("Hit Target Not Found!");
			return EventResult::kContinue;
		}

		if (!hit_target->Is3DLoaded()) {
			logger::debug("Hit Target Not 3D Loaded!");
			return EventResult::kContinue;
		}


		if (hit_target->formType != RE::FormType::ActorCharacter) {
			logger::debug("Hit Target Not Actor!");
			return EventResult::kContinue;
		}

		if (hit_target->IsDead()) {
			logger::debug("Hit Target is Dead!");
			return EventResult::kContinue;
		}


		RE::BSTSmartPointer<RE::BSAnimationGraphManager> animationGraphManagerPtr;
		if (hit_target->GetAnimationGraphManager(animationGraphManagerPtr)) {
			RE::BShkbAnimationGraph* animationGraph = animationGraphManagerPtr->graphs[animationGraphManagerPtr->GetRuntimeData().activeGraph].get();

			float out;
			if (!animationGraph->GetGraphVariableFloat("staggerDirection", out)) {
				logger::debug("Hit Target Has No staggerDirection!");
				return EventResult::kContinue;
			}
		}


		//-------------------------------------------------------------------------------------


		//----------------------Check Magic Effect-----------------------------------------------

		auto this_effect = RE::TESForm::LookupByID<RE::EffectSetting>(a_event->magicEffect);

		if (!this_effect) {
			logger::error("Effect Not Found!");
			return EventResult::kContinue;
		}

		if (this_effect->GetArchetype() == RE::EffectArchetypes::ArchetypeID::kStagger) {
			logger::debug("Effect Is Stagger!");
			logger::debug(FMT_STRING("Caster Name is \"{}\", ID is \"{:x}\""), hit_causer->GetName(), hit_causer->GetFormID());
			logger::debug(FMT_STRING("Hit Target Name is \"{}\", ID is \"{:x}\""), hit_target->GetName(), hit_target->GetFormID());

			static const std::string ModifiedDireName = "Maxsu_modifiedStaggerDirection";

			auto headingAng = hit_target->GetHeadingAngle(hit_causer->GetPosition(), false);
			auto staggerDire = headingAng >= 0.f ? headingAng / 360.f : (360.f + headingAng) / 360.f;  //Convert the heading angle value to "staggerDirection" graph variable float

			if (hit_causer->GetFormID() == hit_target->GetFormID()) {
				logger::debug("Effect Is Self Deliverey!");
				staggerDire = 0.f;		//Set Direction to 0 when self casting
			}

			if (hit_target->As<RE::Actor>()->SetGraphVariableFloat(ModifiedDireName, staggerDire))
				logger::debug("Store Stagger Direction Value Successfully!");
			else
				logger::debug("Fail to Store Stagger Direction Value!");
		}
		//---------------------------------------------------------------------------------------

		return EventResult::kContinue;
	}




}
