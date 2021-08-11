#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	//ƒ|ƒCƒ“ƒ^‹¤’Ê
	class PTR_COMMON {
	protected:
		std::shared_ptr<MAPclass::Map> MAPPTs{ nullptr };	//ˆø‚«Œp‚®
		std::shared_ptr<DXDraw> DrawPts{ nullptr };			//ˆø‚«Œp‚®
		HIT_PASSIVE* Hit_obj_p{ nullptr };
		HIT_BLOOD_PASSIVE* Hit_b_obj_p{ nullptr };
	public:
		virtual void Set_Ptr_Common(std::shared_ptr<MAPclass::Map>& MAPPTs_t, std::shared_ptr<DXDraw>& DrawPts_t, HIT_PASSIVE* Hit_obj_p_t, HIT_BLOOD_PASSIVE* Hit_b_obj_p_t) noexcept {
			DrawPts = DrawPts_t;
			MAPPTs = MAPPTs_t;
			Hit_obj_p = Hit_obj_p_t;
			Hit_b_obj_p = Hit_b_obj_p_t;
		}
		virtual void Dispose_Ptr_Common() noexcept {
			DrawPts.reset();
			MAPPTs.reset();
			Hit_obj_p = nullptr;
			Hit_b_obj_p = nullptr;
		}
	};
};