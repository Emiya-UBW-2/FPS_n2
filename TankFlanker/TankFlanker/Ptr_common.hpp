#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	//ƒ|ƒCƒ“ƒ^‹¤’Ê
	class PTR_COMMON {
	protected:
		std::shared_ptr<MAPclass::Map> MAPPTs{ nullptr };	//ˆø‚«Œp‚®
	public:
		virtual void Set_Ptr_Common(std::shared_ptr<MAPclass::Map>& MAPPTs_t) noexcept {
			MAPPTs = MAPPTs_t;
		}
		virtual void Dispose_Ptr_Common(void) noexcept {
			MAPPTs.reset();
		}
	};
};