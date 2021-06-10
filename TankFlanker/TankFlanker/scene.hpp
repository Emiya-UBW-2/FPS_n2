#pragma once
class Sceneclass : Mainclass {
public:
	class LOAD;
	class SELECT;
	class MAINLOOP;
	//
	class TEMPSCENE {
	private:
		//
	protected:
		//�����p��
		std::unique_ptr<DXDraw, std::default_delete<DXDraw>>* DrawPts;
		std::unique_ptr<OPTION, std::default_delete<OPTION>>* OPTPTs;
		std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>* MAPPTs;
		std::shared_ptr<MAINLOOP>* MAINLOOPscene;
		//��
		std::unique_ptr<DeBuG, std::default_delete<DeBuG>>* DebugPTs;
		//UIsound
		SoundHandle decision;
		SoundHandle cancel;
		SoundHandle cursor;
		//�J����
		cam_info camera_main;
		float fov_base = DX_PI_F / 2;
		//
		bool use_lens = false;
		float lens_zoom = 1.f;
		float lens_size = 1.f;
		//
		VECTOR_ref Shadow_minpos;
		VECTOR_ref Shadow_maxpos;
		VECTOR_ref Light_vec;
		COLOR_F Light_color = GetColorF(0, 0, 0, 0);
		COLOR_F Light_color_ref = GetColorF(0, 0, 0, 0);
	public:
		void Get_ptr(
			std::unique_ptr<DXDraw, std::default_delete<DXDraw>>* DrawPts_t,
			std::unique_ptr<OPTION, std::default_delete<OPTION>>* OPTPTs_t,
			std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>* MAPPTs_t,
			std::shared_ptr<MAINLOOP>* MAINLOOPscene_t,
			std::unique_ptr<DeBuG, std::default_delete<DeBuG>>* DebugPTs_t
		) noexcept {
			DrawPts = DrawPts_t;
			OPTPTs = OPTPTs_t;
			MAPPTs = MAPPTs_t;
			MAINLOOPscene = MAINLOOPscene_t;
			DebugPTs = DebugPTs_t;

			this->decision = SoundHandle::Load("data/audio/shot_2.wav");//
			this->cancel = SoundHandle::Load("data/audio/cancel.wav");
			this->cursor = SoundHandle::Load("data/audio/cursor.wav");
		}


		const VECTOR_ref& get_Shadow_minpos(void) const noexcept { return Shadow_minpos; }
		const VECTOR_ref& get_Shadow_maxpos(void) const noexcept { return Shadow_maxpos; }
		const VECTOR_ref& get_Light_vec(void) const noexcept { return Light_vec; }
		const COLOR_F& get_Light_color(void) const noexcept { return Light_color; }
		const COLOR_F& get_Light_color_ref(void) const noexcept { return Light_color_ref; }
		cam_info& Get_Camera(void) noexcept { return camera_main; }

		virtual void Set(void) noexcept {
			fov_base = deg2rad((*DrawPts)->use_vr ? 120 : (*OPTPTs)->Fov);	//fov
			SetUseMaskScreenFlag(FALSE);//���J�X�^����ʂŃG�t�F�N�g���o�Ȃ��Ȃ邽�ߓ����
			SetMousePoint(deskx / 2, desky / 2);											//
			camera_main.set_cam_info(fov_base, 0.05f, 200.f);//1P
		}
		virtual bool UpDate(void) noexcept {
			return true;
		}
		virtual void Dispose(void) noexcept {
		}
		virtual void UI_Draw(void) noexcept {
		}
		virtual void BG_Draw(void) noexcept {
		}
		virtual void Shadow_Draw_Far(void) noexcept {
		}
		virtual void Shadow_Draw_NearFar(void) noexcept {
		}
		virtual void Shadow_Draw(void) noexcept {
		}
		virtual void Main_Draw(void) noexcept {
		}
		virtual const bool& is_lens(void) { return use_lens; }
		virtual const float& zoom_lens(void) { return lens_zoom; }
		virtual const float& size_lens(void) { return lens_size; }
		virtual void Item_Draw(void) noexcept {
		}
		virtual void LAST_Draw(void) noexcept {
		}
	};
	//
	class LOAD : public TEMPSCENE {
	private:
		//�L�[
		switchs left;
		switchs right;
		//�f�[�^
		GUNPARTs* temp_p = nullptr;
		std::shared_ptr<PLAYERclass::Chara>* mine_ptr = nullptr;
		//
		int preset_select = 0;
		int preset_select_max = 0;
		std::vector<save_c> save_parts;
		std::vector<std::string> save_presets;
		//���̂܂�
		std::unique_ptr<UIclass::UI_LOAD, std::default_delete<UIclass::UI_LOAD>> UIparts;
	public:
		std::string& putout_preset() { return save_presets[preset_select]; }
		//
		LOAD(void) noexcept {
			UIparts = std::make_unique<UIclass::UI_LOAD>();
		}
		void Set(void) noexcept override {
			TEMPSCENE::Set();
			UIparts->Get_ptr(DrawPts, MAPPTs);
			{
				left.ready(false);
				right.ready(false);
			}
			mine_ptr = (*MAINLOOPscene)->Get_Mine();
			{
				save_presets.clear();
				WIN32_FIND_DATA win32fdt;
				HANDLE hFind;
				hFind = FindFirstFile("data/save/*", &win32fdt);
				if (hFind != INVALID_HANDLE_VALUE) {
					do {
						//if ((win32fdt.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (win32fdt.cFileName[0] != '.')) {}
						if ((win32fdt.cFileName[0] != '.')) {
							save_presets.emplace_back(win32fdt.cFileName);
						}
					} while (FindNextFile(hFind, &win32fdt));
				} //else{ return false; }
				FindClose(hFind);
				preset_select_max = int(save_presets.size());
			}
			/*�p�[�c�f�[�^�����[�h*/
			{
				std::fstream file;
				save_parts.clear();
				file.open(("data/save/" + save_presets[0]).c_str(), std::ios::binary | std::ios::in);
				save_c savetmp;
				while (true) {
					file.read((char*)&savetmp, sizeof(savetmp));
					if (file.eof()) {
						break;
					}
					this->save_parts.emplace_back(savetmp);
				}
				file.close();
			}
		}
		bool UpDate(void) noexcept override {
			bool changef = false;
			//���Z
			{
				SetMouseDispFlag(TRUE);
				left.get_in((*mine_ptr)->get_key_().akey);
				right.get_in((*mine_ptr)->get_key_().dkey);
				if (left.push()) {
					--preset_select;
					changef = true;
				}
				if (right.push()) {
					++preset_select;
					changef = true;
				}
				if (preset_select < 0) { preset_select = preset_select_max - 1; }
				if (preset_select > preset_select_max - 1) { preset_select = 0; }
				if (changef) {
					/*�p�[�c�f�[�^�����[�h*/
					std::fstream file;
					save_parts.clear();
					file.open(("data/save/" + save_presets[preset_select]).c_str(), std::ios::binary | std::ios::in);
					save_c savetmp;
					while (true) {
						file.read((char*)&savetmp, sizeof(savetmp));
						if (file.eof()) {
							break;
						}
						this->save_parts.emplace_back(savetmp);
					}
					file.close();
				}
				//
			}
			if ((*mine_ptr)->get_key_().jamp) {
				return false;
			}
			return true;
			//
		}
		void UI_Draw(void) noexcept override {
			UIparts->UI_Draw(*MAINLOOPscene, save_parts, save_presets[preset_select]);
		}
	};
	//
	class SELECT : public TEMPSCENE {
	private:
		//sound
		SoundHandle assemble;
		SoundHandle shot_se;
		SoundHandle slide_se;
		SoundHandle trigger_se;
		//�L�[
		switchs up;
		switchs down;
		switchs left;
		switchs right;
		switchs shot;
		//�f�[�^
		GUNPARTs* parts_p = nullptr;
		//
		size_t parts_cat = SIZE_MAX;
		size_t port_cat = SIZE_MAX;
		//
		size_t port_type = 0;
		PLAYERclass::Chara::g_parts* port_ptr = nullptr;
		std::vector<PLAYERclass::Chara::g_parts*> sight_ptr;
		int sight_p_s = 0;
		//
		int parts_select = 0;
		int parts_select_max = 0;
		int change_select = 0;
		int change_select_max = 0;
		//
		VECTOR_ref viewparts;
		VECTOR_ref viewparts_buf;
		VECTOR_ref camparts;
		float xrad_t = 30.f;
		float yrad_t = -135.f;
		float range_t = 0.25f;
		float range_tgt = 0.25f;
		switchs Rot;
		bool Start_b = true;
		float rate = 1.0f;
		std::shared_ptr<PLAYERclass::Chara>* mine_ptr = nullptr;
		//
		float change_per = 1.f;
		//
		std::vector<save_c> save_parts;
		std::string save_tgt = "1";
		//���̂܂�
		std::unique_ptr<UIclass::UI_CUSTOM, std::default_delete<UIclass::UI_CUSTOM>> UIparts;
	public:
		std::string preset;
	private:
		//�T�C�g���悹�邩���Ȃ�
		bool Set_sight_at(PLAYERclass::Chara::g_parts* ports_ptr) noexcept {
			bool trm = false;
			if (ports_ptr->get_attaching()) {
				for (auto& can : ports_ptr->thisparts->can_attach) {
					for (auto& tgt : *(*MAINLOOPscene)->get_parts_data(EnumGunParts::PARTS_SIGHT)) {
						if (can == tgt.mod.get_name()) { trm = true; }
						if (trm) { break; }
					}
					if (trm) { break; }
				}
			}
			return trm;
		}
		//�p�[�c�̃Z�[�u�ݒ�
		void set_pts_save(size_t chang_t) {
			if (save_parts.size() < size_t(parts_select_max) + 1) {
				save_parts.resize(size_t(parts_select_max) + 1);
			}
			save_parts[parts_select_max].cang_ = chang_t;
			save_parts[parts_select_max].type_ = parts_cat;
			save_parts[parts_select_max].pt_cat_ = port_cat;
			save_parts[parts_select_max].pt_type_ = port_type;
			save_parts[parts_select_max].pt_sel_ = 0;
		}
		//�K�{�i
		void set_pts_haveto(size_t port_type_t, size_t port_cat_t, size_t parts_cat_t) {
			if (parts_select == parts_select_max) {
				port_type = port_type_t;
				port_ptr = (*mine_ptr)->get_parts(port_type);
				port_cat = port_cat_t;
				parts_cat = parts_cat_t;
				{
					change_select_max = (int)(*(*MAINLOOPscene)->get_parts_data(parts_cat)).size();
					change_select = std::clamp(change_select, 0, change_select_max - 1);
					parts_p = &(*(*MAINLOOPscene)->get_parts_data(parts_cat))[change_select];
					viewparts_buf = port_ptr->obj.frame(port_ptr->rail_f[port_cat][0].first);
					(*mine_ptr)->Detach_parts(parts_cat);
					(*mine_ptr)->Attach_parts(parts_p, parts_cat, port_ptr, port_cat);
				}
				set_pts_save(size_t(change_select));
			}
			++parts_select_max;

			//�T�C�g����
			if (Set_sight_at((*mine_ptr)->get_parts(parts_cat_t))) {
				sight_ptr[sight_p_s] = (*mine_ptr)->get_parts(parts_cat_t); ++sight_p_s;
			}
		}
		//��K�{�i
		void Set_chang_needs(size_t pts_cat, size_t pot_cat, int& chang_t, int sel = 0) noexcept {
			change_select_max = (int)(*(*MAINLOOPscene)->get_parts_data(parts_cat)).size() + 1;
			chang_t = std::clamp(chang_t, 0, change_select_max - 1);
			if (chang_t == 0) { parts_p = nullptr; }
			else { parts_p = &(*(*MAINLOOPscene)->get_parts_data(pts_cat))[std::max(chang_t - 1, 0)]; }
			viewparts_buf = port_ptr->obj.frame(port_ptr->rail_f[pot_cat][0].first);
			(*mine_ptr)->Detach_parts(pts_cat, (sel == 0) ? sel : (sel - 1));
			if (chang_t != 0) {
				(*mine_ptr)->Attach_parts(parts_p, pts_cat, port_ptr, pot_cat, sel);
			}
			set_pts_save(size_t(chang_t) - 1);
		}
		void set_pts_need(size_t port_type_t, size_t port_cat_t, size_t parts_cat_t) {
			port_type = port_type_t;
			port_ptr = (*mine_ptr)->get_parts(port_type);
			port_cat = port_cat_t;
			if (port_ptr->rail_f[port_cat][0].first > 0) {
				if (parts_select == parts_select_max) {
					parts_cat = parts_cat_t;
					Set_chang_needs(parts_cat, port_cat, change_select);
				}
				++parts_select_max;
			}
			//�T�C�g����
			if (Set_sight_at((*mine_ptr)->get_parts(parts_cat_t))) {
				sight_ptr[sight_p_s] = (*mine_ptr)->get_parts(parts_cat_t); ++sight_p_s;
			}
		}
		//��K�{�i3
		void set_pts_need_3(size_t port_type_t, size_t port_cat_t, size_t parts_cat_t_1, size_t parts_cat_t_2) {
			port_type = port_type_t;
			port_ptr = (*mine_ptr)->get_parts(port_type);
			port_cat = port_cat_t;
			if (port_ptr->rail_f[port_cat][0].first > 0) {
				if (parts_select == parts_select_max) {
					if (change_select == 0) {
						parts_cat = parts_cat_t_1;
					}
					else {
						parts_cat = (change_select < (*MAINLOOPscene)->get_parts_data(parts_cat_t_1)->size() + 1) ? parts_cat_t_1 : parts_cat_t_2;
					}
					(*mine_ptr)->Detach_parts(parts_cat_t_1);
					(*mine_ptr)->Detach_parts(parts_cat_t_2);
					{
						std::vector<GUNPARTs>&data1 = *(*MAINLOOPscene)->get_parts_data(parts_cat_t_1);
						std::vector<GUNPARTs>&data2 = *(*MAINLOOPscene)->get_parts_data(parts_cat_t_2);

						change_select_max = (int)(data1.size() + 1 + data2.size());
						change_select = std::clamp(change_select, 0, change_select_max - 1);
						if (change_select == 0) { parts_p = nullptr; }
						else {
							if (change_select < data1.size() + 1) { parts_p = &data1[std::max(change_select - 1, 0)]; }
							else { parts_p = &data2[std::max(change_select - 1, 0) - data1.size()]; }
						}
						(*mine_ptr)->Detach_parts(parts_cat);
						if (change_select != 0) {
							(*mine_ptr)->Attach_parts(parts_p, parts_cat, port_ptr, port_cat);
						}
						viewparts_buf = port_ptr->obj.frame(port_ptr->rail_f[port_cat][0].first);
					}
					{
						auto chang_t = change_select;
						if (change_select != 0) {
							if (change_select < (*MAINLOOPscene)->get_parts_data(parts_cat_t_1)->size() + 1) { chang_t = std::max(change_select - 1, 0); }
							else { chang_t = int(std::max(change_select - 1, 0) - (*MAINLOOPscene)->get_parts_data(parts_cat_t_1)->size()); }
						}
						set_pts_save(chang_t);
					}
				}
				++parts_select_max;
			}
		}
		//sight
		void set_pts_need_sight(PLAYERclass::Chara::g_parts* s, int sel) {
			if (s != nullptr) {
				port_type = s->get_type();
				port_ptr = s;
				port_cat = EnumAttachPoint::UPER_RAIL;
				if (port_ptr->rail_f[port_cat][0].first > 0) {
					if (parts_select == parts_select_max) {
						parts_cat = EnumGunParts::PARTS_SIGHT;
						Set_chang_needs(parts_cat, port_cat, change_select, sel);
						if (port_type == EnumGunParts::PARTS_MOUNT) {
							save_parts[parts_select_max].pt_sel_ = 1;//���P
						}
					}
					++parts_select_max;
					return;
				}

				port_cat = EnumAttachPoint::SIDEMOUNT;
				if (port_ptr->rail_f[port_cat][0].first > 0) {
					if (parts_select == parts_select_max) {
						parts_cat = EnumGunParts::PARTS_SIGHT;
						Set_chang_needs(parts_cat, port_cat, change_select, sel);
						if (port_type == EnumGunParts::PARTS_MOUNT) {
							save_parts[parts_select_max].pt_sel_ = 1;//���P
						}
					}
					++parts_select_max;
					return;
				}
			}
		}
	public:
		SELECT(void) noexcept {
			UIparts = std::make_unique<UIclass::UI_CUSTOM>();
			this->assemble = SoundHandle::Load("data/audio/assemble.wav");
		}
		void Set(void) noexcept  override {
			TEMPSCENE::Set();
			UIparts->Get_ptr(DrawPts, MAPPTs);
			{
				up.ready(false);
				down.ready(false);
				left.ready(false);
				right.ready(false);
				shot.ready(false);
				Rot.ready(false);
				parts_p = nullptr;
				parts_cat = SIZE_MAX;
				parts_select = 0;
				change_select = 0;
				parts_select_max = 0;
				change_select_max = 0;
				viewparts_buf.clear();
				camparts.clear();
				xrad_t = 30.f;
				yrad_t = -135.f;
				range_t = 0.25f;
				range_tgt = 0.25f;
				Start_b = true;
				rate = 0.1f;
			}
			{
				mine_ptr = (*MAINLOOPscene)->Get_Mine();
			}
			{
				SetCreateSoundTimeStretchRate(1.f / std::clamp(rate, 0.9f, 1.1f));
				shot_se = SoundHandle::Load((*mine_ptr)->get_audio().shot_path);
				slide_se = SoundHandle::Load((*mine_ptr)->get_audio().slide_path);
				trigger_se = SoundHandle::Load((*mine_ptr)->get_audio().trigger_path);
				SetCreateSoundTimeStretchRate(1.f);
			}
			/*�p�[�c�f�[�^�����[�h*/
			{
				std::fstream file;
				save_parts.clear();
				file.open(("data/save/" + preset).c_str(), std::ios::binary | std::ios::in);
				save_c savetmp;
				while (true) {
					file.read((char*)&savetmp, sizeof(savetmp));
					if (file.eof()) { break; }
					this->save_parts.emplace_back(savetmp);
				}
				file.close();
			}
			/*
			{
				save_parts.clear();
				//magazine
				save_parts.resize(save_parts.size() + 1);
				save_parts.back().cang_ = 0;
				save_parts.back().type_ = EnumGunParts::PARTS_MAGAZINE;
				save_parts.back().pt_cat_ = EnumAttachPoint::MAGAZINE_BASE;
				save_parts.back().pt_type_ = EnumGunParts::PARTS_BASE;
				save_parts.back().pt_sel_ = 0;
				//grip
				save_parts.resize(save_parts.size() + 1);
				save_parts.back().cang_ = 0;
				save_parts.back().type_ = EnumGunParts::PARTS_GRIP;
				save_parts.back().pt_cat_ = EnumAttachPoint::GRIP_BASE;
				save_parts.back().pt_type_ = EnumGunParts::PARTS_BASE;
				save_parts.back().pt_sel_ = 0;
				//uperhandguard
				save_parts.resize(save_parts.size() + 1);
				save_parts.back().cang_ = 0;
				save_parts.back().type_ = EnumGunParts::PARTS_UPER_HGUARD;
				save_parts.back().pt_cat_ = EnumAttachPoint::UPER_HANDGUARD;
				save_parts.back().pt_type_ = EnumGunParts::PARTS_BASE;
				save_parts.back().pt_sel_ = 0;
				//underhandguard
				save_parts.resize(save_parts.size() + 1);
				save_parts.back().cang_ = 0;
				save_parts.back().type_ = EnumGunParts::PARTS_UNDER_HGUARD;
				save_parts.back().pt_cat_ = EnumAttachPoint::UNDER_HANDGUARD;
				save_parts.back().pt_type_ = EnumGunParts::PARTS_BASE;
				save_parts.back().pt_sel_ = 0;
				//�}�Y��
				save_parts.resize(save_parts.size() + 1);
				save_parts.back().cang_ = 0;
				save_parts.back().type_ = EnumGunParts::PARTS_MAZZULE;
				save_parts.back().pt_cat_ = EnumAttachPoint::MAZZULE_BASE;
				save_parts.back().pt_type_ = EnumGunParts::PARTS_BASE;
				save_parts.back().pt_sel_ = 0;
				//�_�X�g�J�o�[
				save_parts.resize(save_parts.size() + 1);
				save_parts.back().cang_ = 0;
				save_parts.back().type_ = EnumGunParts::PARTS_DUSTCOVER;
				save_parts.back().pt_cat_ = EnumAttachPoint::DUSTCOVER_BASE;
				save_parts.back().pt_type_ = EnumGunParts::PARTS_BASE;
				save_parts.back().pt_sel_ = 0;
				//�X�g�b�N
				save_parts.resize(save_parts.size() + 1);
				save_parts.back().cang_ = 0;
				save_parts.back().type_ = EnumGunParts::PARTS_STOCK;
				save_parts.back().pt_cat_ = EnumAttachPoint::STOCK_BASE;
				save_parts.back().pt_type_ = EnumGunParts::PARTS_BASE;
				save_parts.back().pt_sel_ = 0;
			}
			//*/
			//�ǂݍ���
			{
				int pppp = 0;
				for (auto&tmp_save : save_parts) {
					GUNPARTs* temp_p = nullptr;
					PLAYERclass::Chara::g_parts* temp_ptr = nullptr;
					if (tmp_save.cang_ != SIZE_MAX) {
						auto vec_data = (*MAINLOOPscene)->get_parts_data(tmp_save.type_);
						if (vec_data != nullptr) {
							temp_p = &(*vec_data)[tmp_save.cang_];
						}
					}
					{
						switch (tmp_save.pt_type_) {
						case EnumGunParts::PARTS_BASE:
						case EnumGunParts::PARTS_UPER_HGUARD:
						case EnumGunParts::PARTS_UNDER_HGUARD:
						case EnumGunParts::PARTS_DUSTCOVER:
						case EnumGunParts::PARTS_MOUNT_BASE:
						case EnumGunParts::PARTS_MOUNT:
							temp_ptr = (*mine_ptr)->get_parts(tmp_save.pt_type_);
							break;
						default:
							break;
						}
					}
					if (tmp_save.type_ == EnumGunParts::PARTS_SIGHT) {
						++pppp;
					}
					(*mine_ptr)->Attach_parts(temp_p, tmp_save.type_, temp_ptr, tmp_save.pt_cat_, pppp);
				}
				//save_parts.clear();
			}
			//���C�e�B���O
			Shadow_maxpos = VECTOR_ref::vget(1.f, 1.f, 1.f);
			Shadow_minpos = VECTOR_ref::vget(-1.f, -1.f, -1.f);
			Light_vec = VECTOR_ref::vget(0.5f, -0.5f, 0.5f);
			Light_color = GetColorF(0.42f, 0.41f, 0.40f, 0.0f);
			Light_color_ref = GetColorF(0.20f, 0.20f, 0.23f, 0.0f);
		}
		bool UpDate(void) noexcept override {
			bool changef = false;
			//���Z
			{
				{
					moves tmp;
					(*mine_ptr)->set_gun_pos(tmp);
					(*mine_ptr)->Set_gun();
					(*mine_ptr)->set_mag_pos();
					(*mine_ptr)->get_parts(EnumGunParts::PARTS_MAGAZINE)->Setpos_Nomal((*mine_ptr)->Mag_Mat());
					//�L�[
					up.get_in((*mine_ptr)->get_key_().wkey);
					down.get_in((*mine_ptr)->get_key_().skey);
					left.get_in((*mine_ptr)->get_key_().akey);
					right.get_in((*mine_ptr)->get_key_().dkey);
					shot.get_in((*mine_ptr)->get_key_().shoot);
					Rot.get_in((*mine_ptr)->get_key_().select);
					//
					if (Start_b) {
						Start_b = false;
						changef = true;
						int pp = (*mine_ptr)->get_parts(EnumGunParts::PARTS_BASE)->thisparts->Select_Chose(EnumSELECTER::SELECT_SEMI);
						if (pp != -1) {
							(*mine_ptr)->gun_stat_now->selector_set(pp);
						}
					}
					if (left.push()) {
						parts_select--;
						change_select = 0;
						changef = true;
					}
					if (right.push()) {
						++parts_select;
						change_select = 0;
						changef = true;
					}
					if (parts_select < 0) { parts_select = parts_select_max - 1; }
					if (parts_select > parts_select_max - 1) { parts_select = 0; }

					if (up.push()) {
						change_select--;
						changef = true;
					}
					if (down.push()) {
						++change_select;
						changef = true;
					}
					if (change_select < 0) { change_select = change_select_max - 1; }
					if (change_select > change_select_max - 1) { change_select = 0; }
				}
				if (changef) {
					{
						Rot.first = false;
						port_ptr = nullptr;
						parts_select_max = 0;
						//�T�C�g����
						sight_ptr.resize(5);
						for (auto& s : sight_ptr) {
							s = nullptr;
						}
						sight_p_s = 0;
					}
					set_pts_haveto(EnumGunParts::PARTS_BASE, EnumAttachPoint::MAGAZINE_BASE, EnumGunParts::PARTS_MAGAZINE);			//magazine
					set_pts_haveto(EnumGunParts::PARTS_BASE, EnumAttachPoint::GRIP_BASE, EnumGunParts::PARTS_GRIP);					//grip
					set_pts_haveto(EnumGunParts::PARTS_BASE, EnumAttachPoint::UPER_HANDGUARD, EnumGunParts::PARTS_UPER_HGUARD);		//uperhandguard
					set_pts_haveto(EnumGunParts::PARTS_BASE, EnumAttachPoint::UNDER_HANDGUARD, EnumGunParts::PARTS_UNDER_HGUARD);	//underhandguard
					set_pts_need(EnumGunParts::PARTS_UNDER_HGUARD, EnumAttachPoint::UNDER_RAIL, EnumGunParts::PARTS_FOREGRIP);		//foregrip
					set_pts_need(EnumGunParts::PARTS_UNDER_HGUARD, EnumAttachPoint::LEFTSIDE_RAIL, EnumGunParts::PARTS_LAM);		//lam
					set_pts_need(EnumGunParts::PARTS_BASE, EnumAttachPoint::SIDEMOUNT_BASE, EnumGunParts::PARTS_MOUNT_BASE);		//sidemount
					set_pts_need(EnumGunParts::PARTS_MOUNT_BASE, EnumAttachPoint::SIDEMOUNT, EnumGunParts::PARTS_MOUNT);			//sidemount_2
					set_pts_need(EnumGunParts::PARTS_BASE, EnumAttachPoint::MAZZULE_BASE, EnumGunParts::PARTS_MAZZULE);				//�}�Y��
					set_pts_need(EnumGunParts::PARTS_BASE, EnumAttachPoint::DUSTCOVER_BASE, EnumGunParts::PARTS_DUSTCOVER);			//�_�X�g�J�o�[
					set_pts_need(EnumGunParts::PARTS_BASE, EnumAttachPoint::STOCK_BASE, EnumGunParts::PARTS_STOCK);					//�X�g�b�N
					//sight
					{
						int ssp = parts_select_max;
						for (auto& s : sight_ptr) {
							set_pts_need_sight(s, (parts_select_max - ssp + 1));
						}
					}
					//
					{
						change_per = 1.f;
						assemble.play(DX_PLAYTYPE_BACK, TRUE);

						xrad_t = viewparts_buf.y()*1000.f;
						yrad_t = ((viewparts_buf.x() > 0.f) ? 90.f - viewparts_buf.z()*100.f : -90.f + viewparts_buf.z()*100.f);
					}
				}
				//
				easing_set(&(*mine_ptr)->get_gunanime_trigger()->per, shot.press(), 0.5f);
				if (shot.push()) {
					if ((*mine_ptr)->set_gunf()) {
						//todo �f�B���C����
						{
							shot_se.play(DX_PLAYTYPE_BACK, TRUE);
							slide_se.play(DX_PLAYTYPE_BACK, TRUE);
							trigger_se.play(DX_PLAYTYPE_BACK, TRUE);
							//���
							(*mine_ptr)->create_cart();
						}
						//�G�t�F�N�g
						(*mine_ptr)->calc_shot_effect();
					}
				}
				//���
				(*mine_ptr)->calc_cart(rate);
				//�e�Z���N�^�[�A�j��
				(*mine_ptr)->Set_select_anime();
				//�e���C�A�j��
				(*mine_ptr)->Set_shot_anime(rate, true);
				//�e�A�j���X�V
				(*mine_ptr)->get_parts(EnumGunParts::PARTS_BASE)->obj.work_anime();
				//��䰂̏���
				(*mine_ptr)->update_cart();
				//�G�t�F�N�g�̍X�V
				(*mine_ptr)->update_effect();
			}
			//campos,camvec,camup�̎w��
			{}
			{
				{
					int x_m, y_m;
					GetMousePoint(&x_m, &y_m);
					x_m -= deskx / 2;
					y_m -= desky / 2;
					SetMousePoint(deskx / 2, desky / 2);
					SetMouseDispFlag(FALSE);
					xrad_t = std::clamp(xrad_t + float(std::clamp(y_m, -60, 60))*0.1f, -80.f, 80.f);
					yrad_t += float(std::clamp(x_m, -60, 60))*0.1f;
					yrad_t = rad2deg(atan2f(sin(deg2rad(yrad_t)), cos(deg2rad(yrad_t))));
				}
				{
					if (Rot.on()) {
						viewparts.clear();
					}
					else {
						viewparts = viewparts_buf;
					}
				}
				range_tgt = std::hypotf(sin(deg2rad(yrad_t))*0.25f, cos(deg2rad(yrad_t))*(std::abs((((std::abs(yrad_t) > 90) ? (*mine_ptr)->get_maz().z() : 0.5f) - viewparts.z())) + camera_main.near_ * 2.f));
				range_t = (Rot.on() && !changef) ? std::clamp(range_t - float(GetMouseWheelRotVol())*0.1f, range_tgt, 5.f) : range_tgt;

				easing_set(&camparts, VECTOR_ref::vget(range_t*cos(deg2rad(xrad_t))*sin(deg2rad(yrad_t)), range_t*sin(deg2rad(xrad_t)), range_t*cos(deg2rad(xrad_t))*cos(deg2rad(yrad_t))), 0.8f);
				camera_main.camvec = viewparts;
				camera_main.campos = camera_main.camvec + camparts;
				camera_main.camup = VECTOR_ref::vget(0, 1.f, 0);
			}
			//�\���̔@��
			{
				cam_info* camp = &this->camera_main;
				GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), camp->campos, camp->camvec, camp->camup, camp->fov, camp->near_, camp->far_);
				{
					(*mine_ptr)->Check_CameraViewClip(false);
				}
			}
			if ((*mine_ptr)->get_key_().jamp) {
				return false;
			}
			return true;
			//
		}
		void Dispose(void) noexcept override {
			//�f�[�^�Z�[�u
			{
				std::fstream file;
				file.open(("data/save/" + save_tgt + ".dat").c_str(), std::ios::binary | std::ios::out);
				for (auto&tmp_save : save_parts) {
					file.write((char*)&tmp_save, sizeof(tmp_save));
				}
				file.close();
			}
			(*mine_ptr)->gun_stat_now->selector_set(0);
			shot_se.Dispose();
			slide_se.Dispose();
			trigger_se.Dispose();
		}
		void UI_Draw(void) noexcept  override {
			UIparts->UI_Draw(*MAINLOOPscene, parts_cat, Rot.on(), mine_ptr, parts_p, change_per);
			easing_set(&change_per, 0.f, 0.5f);
		}
		void BG_Draw(void) noexcept override {
			DrawBox(0, 0, 1920, 1080, GetColor(192, 192, 192), TRUE);
		}
		void Shadow_Draw_NearFar(void) noexcept override {
			(*mine_ptr)->Draw_gun();
			{
				(*mine_ptr)->get_parts(EnumGunParts::PARTS_MAGAZINE)->Draw();
			}
		}
		void Shadow_Draw(void) noexcept override {
			(*mine_ptr)->Draw_gun();
			{
				(*mine_ptr)->get_parts(EnumGunParts::PARTS_MAGAZINE)->Draw();
			}
		}
		void Main_Draw(void) noexcept override {
			(*mine_ptr)->Draw_gun();
			{
				(*mine_ptr)->get_parts(EnumGunParts::PARTS_MAGAZINE)->Draw();
			}
		}
	};
	//
	class MAINLOOP : public TEMPSCENE {
		//TPS����
		class TPS_parts {
			std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>* MAPPTs;
		public:
			switchs key_TPS;
			float xr_cam = 0.f;
			float yr_cam = 0.f;
			int sel_cam = 0;
			//�J����
			cam_info camera_TPS;
			TPS_parts(std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>* MAPPTs_t) noexcept {
				MAPPTs = MAPPTs_t;
			}
			void Set(float& fov_pc) noexcept {
				this->key_TPS.ready(false);
				this->camera_TPS.campos = VECTOR_ref::vget(0, 10.f, -10);
				this->camera_TPS.set_cam_info(deg2rad(fov_pc), 0.1f, 200.f);
			}
			void Set_info(std::vector<std::shared_ptr<PLAYERclass::Chara>>&chara) noexcept {
				if (this->key_TPS.on()) {
					{
						//cam
						for (int i = 0; i < std::min<size_t>(chara.size(), 10); ++i) {
							if (CheckHitKey(KEY_INPUT_1 + i) != 0) {
								this->sel_cam = i;
							}
						}
						//pos
						{
							if (CheckHitKey(KEY_INPUT_LEFT) != 0) {
								this->camera_TPS.campos.x(this->camera_TPS.campos.x() - 10.f / FPS * cos(this->yr_cam));
								this->camera_TPS.campos.z(this->camera_TPS.campos.z() + 10.f / FPS * sin(this->yr_cam));
							}
							if (CheckHitKey(KEY_INPUT_RIGHT) != 0) {
								this->camera_TPS.campos.x(this->camera_TPS.campos.x() + 10.f / FPS * cos(this->yr_cam));
								this->camera_TPS.campos.z(this->camera_TPS.campos.z() - 10.f / FPS * sin(this->yr_cam));
							}
							if (CheckHitKey(KEY_INPUT_UP) != 0) {
								this->camera_TPS.campos.z(this->camera_TPS.campos.z() + 10.f / FPS * cos(this->yr_cam));
								this->camera_TPS.campos.x(this->camera_TPS.campos.x() + 10.f / FPS * sin(this->yr_cam));
							}
							if (CheckHitKey(KEY_INPUT_DOWN) != 0) {
								this->camera_TPS.campos.z(this->camera_TPS.campos.z() - 10.f / FPS * cos(this->yr_cam));
								this->camera_TPS.campos.x(this->camera_TPS.campos.x() - 10.f / FPS * sin(this->yr_cam));
							}
							this->camera_TPS.campos.x(std::clamp(this->camera_TPS.campos.x(), (*MAPPTs)->map_col_get().mesh_minpos(0).x(), (*MAPPTs)->map_col_get().mesh_maxpos(0).x()));
							this->camera_TPS.campos.z(std::clamp(this->camera_TPS.campos.z(), (*MAPPTs)->map_col_get().mesh_minpos(0).z(), (*MAPPTs)->map_col_get().mesh_maxpos(0).z()));
						}
						//rad
						{
							VECTOR_ref vec_2 = (chara[this->sel_cam]->get_head_pos() - this->camera_TPS.campos).Norm();
							VECTOR_ref vec_z = (this->camera_TPS.camvec - this->camera_TPS.campos).Norm();
							VECTOR_ref vec_x = vec_z.cross(this->camera_TPS.camup);

							this->xr_cam -= deg2rad(int(vec_z.cross(vec_2).dot(vec_x) * 50))*0.1f;
							this->yr_cam -= deg2rad(int(vec_x.dot(vec_2) * 50))*0.1f;
							this->xr_cam = std::clamp(this->xr_cam, deg2rad(-89), deg2rad(89));
						}
						//
						this->camera_TPS.camvec = this->camera_TPS.campos + MATRIX_ref::Vtrans(VECTOR_ref::vget(0, 0, 1), MATRIX_ref::RotX(this->xr_cam)*MATRIX_ref::RotY(this->yr_cam));
						this->camera_TPS.camup = VECTOR_ref::vget(0, 1.f, 0);
					}
				}
			}
		};
		//�f�[�^
		MV1 body_obj, body_obj_lag, body_col;		//�g�̃��f��
		MV1 hit_pic;								//�e��
		GraphHandle light;							//���C�g�e�N�X�`��
		std::vector<GUNPARTs> mazzule_data;			//GUN�f�[�^
		std::vector<GUNPARTs> grip_data;			//GUN�f�[�^
		std::vector<GUNPARTs> uperhandguard_data;	//GUN�f�[�^
		std::vector<GUNPARTs> underhandguard_data;	//GUN�f�[�^
		std::vector<GUNPARTs> stock_data;			//GUN�f�[�^
		std::vector<GUNPARTs> dustcover_data;		//GUN�f�[�^
		std::vector<GUNPARTs> sight_data;			//GUN�f�[�^
		std::vector<GUNPARTs> foregrip_data;		//GUN�f�[�^
		std::vector<GUNPARTs> lam_data;				//GUN�f�[�^
		std::vector<GUNPARTs> mount_base_data;		//GUN�f�[�^
		std::vector<GUNPARTs> mount_data;			//GUN�f�[�^
		std::vector<GUNPARTs> gun_data;				//GUN�f�[�^
		std::vector<GUNPARTs> magazine_data;		//GUN�f�[�^
		std::vector<Meds> meds_data;				//GUN�f�[�^
		std::vector<Grenades> gres_data;			//GUN�f�[�^
		std::vector<std::shared_ptr<PLAYERclass::Chara>> chara;		//�L����
		HIT_PASSIVE hit_obj_p;						//�ÓI�e��
		HIT_BLOOD_PASSIVE hit_b_obj_p;				//�ÓI����
		//���̂܂�
		std::unique_ptr<HostPassEffect, std::default_delete<HostPassEffect>> Hostpassparts_TPS;
		std::unique_ptr<TPS_parts, std::default_delete<TPS_parts>> TPSparts;
		std::unique_ptr<UIclass::UI_MAINLOOP, std::default_delete<UIclass::UI_MAINLOOP>> UIparts;
		std::unique_ptr<RULE_parts, std::default_delete<RULE_parts>> RULEparts;
		std::unique_ptr<MAPclass::MiniMap, std::default_delete<MAPclass::MiniMap>> minimapparts;
	public:
		std::shared_ptr<PLAYERclass::Chara>* Get_Mine(void) noexcept { return &this->chara[0]; }
		std::vector<GUNPARTs>* get_parts_data(size_t type_sel) noexcept {
			switch (type_sel) {
			case EnumGunParts::PARTS_MAGAZINE:
				return &this->magazine_data;
			case EnumGunParts::PARTS_GRIP:
				return &this->grip_data;
			case EnumGunParts::PARTS_UPER_HGUARD:
				return &this->uperhandguard_data;
			case EnumGunParts::PARTS_UNDER_HGUARD:
				return &this->underhandguard_data;
			case EnumGunParts::PARTS_MAZZULE:
				return &this->mazzule_data;
			case EnumGunParts::PARTS_DUSTCOVER:
				return &this->dustcover_data;
			case EnumGunParts::PARTS_STOCK:
				return &this->stock_data;
			case EnumGunParts::PARTS_LAM:
				return &this->lam_data;
			case EnumGunParts::PARTS_MOUNT_BASE:
				return &this->mount_base_data;
			case EnumGunParts::PARTS_MOUNT:
				return &this->mount_data;
			case EnumGunParts::PARTS_SIGHT:
				return &this->sight_data;
			case EnumGunParts::PARTS_FOREGRIP:
				return &this->foregrip_data;
			default:
				break;
			}
			return nullptr;
		}
		std::vector<Meds>& get_meds_data(void) noexcept { return this->meds_data; }
		std::vector<Grenades>& get_gres_data(void) noexcept { return this->gres_data; }
		void set_parts_data_from_folder(std::vector<GUNPARTs>* data, std::string file_name) noexcept {
			data->clear();
			std::string p;
			WIN32_FIND_DATA win32fdt;
			HANDLE hFind;
			hFind = FindFirstFile((file_name + "*").c_str(), &win32fdt);
			if (hFind != INVALID_HANDLE_VALUE) {
				do {
					if ((win32fdt.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (win32fdt.cFileName[0] != '.')) {
						data->resize(data->size() + 1);
						data->back().mod.Ready(file_name, win32fdt.cFileName);
					}
				} while (FindNextFile(hFind, &win32fdt));
			} //else{ return false; }
			FindClose(hFind);
		}
		void set_mads_data_from_folder(std::vector<Meds>* data, std::string file_name) noexcept {
			data->clear();
			std::string p;
			WIN32_FIND_DATA win32fdt;
			HANDLE hFind;
			hFind = FindFirstFile((file_name + "*").c_str(), &win32fdt);
			if (hFind != INVALID_HANDLE_VALUE) {
				do {
					if ((win32fdt.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (win32fdt.cFileName[0] != '.')) {
						data->resize(data->size() + 1);
						data->back().mod.Ready(file_name, win32fdt.cFileName);
					}
				} while (FindNextFile(hFind, &win32fdt));
			} //else{ return false; }
			FindClose(hFind);
		}
		void set_gres_data_from_folder(std::vector<Grenades>* data, std::string file_name) noexcept {
			data->clear();
			std::string p;
			WIN32_FIND_DATA win32fdt;
			HANDLE hFind;
			hFind = FindFirstFile((file_name + "*").c_str(), &win32fdt);
			if (hFind != INVALID_HANDLE_VALUE) {
				do {
					if ((win32fdt.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (win32fdt.cFileName[0] != '.')) {
						data->resize(data->size() + 1);
						data->back().mod.Ready(file_name, win32fdt.cFileName);
					}
				} while (FindNextFile(hFind, &win32fdt));
			} //else{ return false; }
			FindClose(hFind);
		}
	public:
		MAINLOOP(std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>* MAPPTs_t, std::unique_ptr<OPTION, std::default_delete<OPTION>>* OPTPTs_t) noexcept {
			//
			MAPPTs = MAPPTs_t;
			OPTPTs = OPTPTs_t;
			//
			UIparts = std::make_unique<UIclass::UI_MAINLOOP>(&RULEparts);
			TPSparts = std::make_unique<TPS_parts>(MAPPTs);
			RULEparts = std::make_unique<RULE_parts>();
			Hostpassparts_TPS = std::make_unique<HostPassEffect>((*OPTPTs)->DoF, (*OPTPTs)->Bloom, (*OPTPTs)->SSAO, deskx, desky);	//�z�X�g�p�X�G�t�F�N�g(�t���X�N���[�������ATPS�p)
			minimapparts = std::make_unique<MAPclass::MiniMap>(MAPPTs);															//�~�j�}�b�v
			//model
			light = GraphHandle::Load("data/light.png");							//���C�g
			MV1::Load("data/model/hit/model.mv1", &this->hit_pic, true);			//�e��
			//body
			MV1::Load("data/model/body/model.mv1", &this->body_obj, true);			//�g��
			MV1::Load("data/model/body/col.mv1", &this->body_col, true);			//�g��col
			MV1SetLoadModelPhysicsWorldGravity(-1.f);
			MV1::Load("data/model/body/model_lag.mv1", &this->body_obj_lag, true);	//�g��
			MV1SetLoadModelPhysicsWorldGravity(-9.8f);
			//PARTS�f�[�^
			set_parts_data_from_folder(&this->mazzule_data, "data/parts/mazzule/");
			set_parts_data_from_folder(&this->grip_data, "data/parts/grip/");
			set_parts_data_from_folder(&this->uperhandguard_data, "data/parts/uper_handguard/");
			set_parts_data_from_folder(&this->underhandguard_data, "data/parts/under_handguard/");
			set_parts_data_from_folder(&this->mount_base_data, "data/parts/mount_base/");
			set_parts_data_from_folder(&this->mount_data, "data/parts/mount/");
			set_parts_data_from_folder(&this->stock_data, "data/parts/stock/");
			set_parts_data_from_folder(&this->dustcover_data, "data/parts/dustcover/");
			set_parts_data_from_folder(&this->sight_data, "data/parts/sight/");
			set_parts_data_from_folder(&this->foregrip_data, "data/parts/foregrip/");
			set_parts_data_from_folder(&this->lam_data, "data/parts/lam/");
			//GUN�f�[�^
			set_parts_data_from_folder(&this->gun_data, "data/gun/");
			//MAG�f�[�^
			set_parts_data_from_folder(&this->magazine_data, "data/mag/");
			//MED�f�[�^
			set_mads_data_from_folder(&this->meds_data, "data/medkit/");
			//�O���l�[�h
			set_gres_data_from_folder(&this->gres_data, "data/grenade/");
			//
			this->hit_obj_p.init();
			this->hit_b_obj_p.init();
		}
		void Start_After(void) noexcept {
			//PARTS�f�[�^2
			for (auto& g : this->mazzule_data) { g.Set_datas(&g - &this->mazzule_data[0], EnumGunParts::PARTS_MAZZULE); }
			for (auto& g : this->grip_data) { g.Set_datas(&g - &this->grip_data[0], EnumGunParts::PARTS_NONE); }
			for (auto& g : this->uperhandguard_data) { g.Set_datas(&g - &this->uperhandguard_data[0], EnumGunParts::PARTS_NONE); }
			for (auto& g : this->underhandguard_data) { g.Set_datas(&g - &this->underhandguard_data[0], EnumGunParts::PARTS_NONE); }
			for (auto& g : this->mount_base_data) { g.Set_datas(&g - &this->mount_base_data[0], EnumGunParts::PARTS_NONE); }
			for (auto& g : this->mount_data) { g.Set_datas(&g - &this->mount_data[0], EnumGunParts::PARTS_NONE); }
			for (auto& g : this->stock_data) { g.Set_datas(&g - &this->stock_data[0], EnumGunParts::PARTS_STOCK); }
			for (auto& g : this->dustcover_data) { g.Set_datas(&g - &this->dustcover_data[0], EnumGunParts::PARTS_NONE); }
			for (auto& g : this->lam_data) { g.Set_datas(&g - &this->lam_data[0], EnumGunParts::PARTS_LAM); }
			for (auto& g : this->foregrip_data) { g.Set_datas(&g - &this->foregrip_data[0], EnumGunParts::PARTS_NONE); }
			for (auto& g : this->sight_data) { g.Set_datas(&g - &this->sight_data[0], EnumGunParts::PARTS_SIGHT); }
			//GUN�f�[�^2
			for (auto& g : this->gun_data) { g.Set_datas(&g - &this->gun_data[0], EnumGunParts::PARTS_BASE); }
			//MAG�f�[�^2
			for (auto& g : this->magazine_data) { g.Set_datas(&g - &this->magazine_data[0], EnumGunParts::PARTS_MAGAZINE); }
			//MED�f�[�^2
			for (auto& g : this->meds_data) { g.Set_datas(&g - &this->meds_data[0]); }
			//MED�f�[�^2
			for (auto& g : this->gres_data) { g.Set_datas(&g - &this->gres_data[0]); }
			//�e��
			this->hit_obj_p.clear();
			this->hit_b_obj_p.clear();
		}
		void Set_Charaa(const size_t &spawn_total) noexcept {
			//�L�����ݒ�
			this->chara.resize(spawn_total);
			for (auto& c : this->chara) { c = std::make_shared<PLAYERclass::Chara>(); }
			for (auto& c : this->chara) { c->Get_ptr(MAPPTs, DrawPts, DebugPTs, &c); }
			for (auto& c : this->chara) { c->Set_(gun_data, 0, body_obj, body_obj_lag, body_col); }
		}
	public:
		void Set(void) noexcept override {
			TEMPSCENE::Set();
			UIparts->Get_ptr(DrawPts, MAPPTs);
			//
			for (auto& c : this->chara) {
				//�J�X�^��attach
				if (&c != Get_Mine()) {
					//magazine
					c->Attach_parts(&magazine_data[0], EnumGunParts::PARTS_MAGAZINE, nullptr, 0);
					//grip
					c->Attach_parts(&grip_data[0], EnumGunParts::PARTS_GRIP, c->get_parts(EnumGunParts::PARTS_BASE), EnumAttachPoint::GRIP_BASE);
					//uperhandguard
					c->Attach_parts(&uperhandguard_data[0], EnumGunParts::PARTS_UPER_HGUARD, c->get_parts(EnumGunParts::PARTS_BASE), EnumAttachPoint::UPER_HANDGUARD);
					//underhandguard
					c->Attach_parts(&underhandguard_data[0], EnumGunParts::PARTS_UNDER_HGUARD, c->get_parts(EnumGunParts::PARTS_BASE), EnumAttachPoint::UNDER_HANDGUARD);
					//sidemount + sight
					c->Attach_parts(&mount_base_data[0], EnumGunParts::PARTS_MOUNT_BASE, c->get_parts(EnumGunParts::PARTS_BASE), EnumAttachPoint::SIDEMOUNT_BASE);
					c->Attach_parts(&mount_data[0], EnumGunParts::PARTS_MOUNT, c->get_parts(EnumGunParts::PARTS_MOUNT_BASE), EnumAttachPoint::SIDEMOUNT);
					c->Attach_parts(&sight_data[0], EnumGunParts::PARTS_SIGHT, c->get_parts(EnumGunParts::PARTS_MOUNT), EnumAttachPoint::UPER_RAIL, 1);
					//optional
					c->Attach_parts(&mazzule_data[0], EnumGunParts::PARTS_MAZZULE, c->get_parts(EnumGunParts::PARTS_BASE), EnumAttachPoint::MAZZULE_BASE);
					c->Attach_parts(&dustcover_data[0], EnumGunParts::PARTS_DUSTCOVER, c->get_parts(EnumGunParts::PARTS_BASE), EnumAttachPoint::DUSTCOVER_BASE);
					c->Attach_parts(&stock_data[0], EnumGunParts::PARTS_STOCK, c->get_parts(EnumGunParts::PARTS_BASE), EnumAttachPoint::STOCK_BASE);
				}
				//����ʒu�ݒ�X�|�[��
				{
					auto&wp = (*MAPPTs)->get_spawn_point()[&c - &this->chara[0]];
					moves tmp;
					tmp.pos = wp;
					tmp.mat = MATRIX_ref::RotY(atan2f(wp.x(), wp.z()));
					c->Spawn(tmp);
				}
				//�v���C���[����ϐ��Q
				c->Start();
			}
			TPSparts->Set((*OPTPTs)->Fov);						//TPS
			RULEparts->Set();									//���[��
			//���C�e�B���O
			Shadow_maxpos = (*MAPPTs)->map_col_get().mesh_maxpos(0);
			Shadow_minpos = (*MAPPTs)->map_col_get().mesh_minpos(0);
			Light_vec = VECTOR_ref::vget(0.5f, -0.5f, 0.5f);
			Light_color = GetColorF(0.42f, 0.41f, 0.40f, 0.0f);
			Light_color_ref = GetColorF(0.20f, 0.20f, 0.23f, 0.0f);
			(*MAPPTs)->Set();									//��
		}
		bool UpDate(void) noexcept override {
			//�A�C�e�����Z
			{
				auto& item = (*MAPPTs)->item;
				for (auto& g : item) { g->UpDate((*MAPPTs)->item, (*MAPPTs)); }
				//���Ԃ��o�����}�K�W�����폜����
				bool fal = true;
				while (true) {
					fal = true;
					for (auto& i : item) {
						auto now = &i - &item.front();
						if (i->Detach_mag()) {
							std::iter_swap(item.begin() + now, item.end() - 1);
							item.pop_back();
							fal = false;
							break;
						}
						if (i->Detach_gre((*MAPPTs), &this->chara[0], &this->chara)) {
							std::iter_swap(item.begin() + now, item.end() - 1);
							item.pop_back();
							fal = false;
							break;
						}
					}
					if (fal) {
						break;
					}
				}
			}
			//���ʉ��Z//2�`3ms
			for (auto& c : this->chara) {
				c->UpDate_(this->hit_obj_p, this->hit_b_obj_p, RULEparts->get_Playing(), this->camera_main.fov / this->fov_base, this->meds_data, this->gres_data, &this->chara, this->Get_Mine());
			}
			//�e���̍X�V
			this->hit_obj_p.update();
			this->hit_b_obj_p.update();
			//campos,camvec,camup�̎w��
			(*this->Get_Mine())->Set_cam(this->camera_main, &this->chara, this->fov_base);
			//���[���ۑ�
			UIparts->Update();
			RULEparts->UpDate();
			//TPS�`��
			TPSparts->key_TPS.get_in((*this->Get_Mine())->get_key_().TPS);
			TPSparts->Set_info(this->chara);
			if (TPSparts->key_TPS.on()) {
				//�e�p��
				(*DrawPts)->Ready_Shadow(TPSparts->camera_TPS.campos, [&] {Shadow_Draw(); }, [&] {Shadow_Draw_NearFar(); }, VECTOR_ref::vget(2.f, 2.5f, 2.f), VECTOR_ref::vget(15.f, 12.5f, 15.f));
				//��������
				Hostpassparts_TPS->BUF_draw([&](void) noexcept { BG_Draw(); }, [&] {(*DrawPts)->Draw_by_Shadow([&] {Main_Draw(); }); }, TPSparts->camera_TPS);	//��ʑ̐[�x�`��
				Hostpassparts_TPS->Set_MAIN_draw();																												//�ŏI�`��
			}
			//�\���̔@��
			{
				cam_info* camp = TPSparts->key_TPS.on() ? &TPSparts->camera_TPS : &this->camera_main;
				GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), camp->campos, camp->camvec, camp->camup, camp->fov, camp->near_, camp->far_);
				{
					for (auto& c : this->chara) {
						c->Check_CameraViewClip(false);
					}
					for (auto& i : (*MAPPTs)->item) {
						i->Check_CameraViewClip(*MAPPTs, false);
					}

					(*MAPPTs)->Check_CameraViewClip(*MAPPTs, false);
				}
			}
			//�I������
			if (RULEparts->get_timer() <= -5.f) {
				return false;
			}
			return true;
		}
		void Dispose(void) noexcept override {
			for (auto& c : this->chara) {
				c->Dispose();
			}
			this->chara.clear();
		}
		void UI_Draw(void) noexcept  override {
			UIparts->UI_Draw(this->Get_Mine());
			//�~�j�}�b�v
			minimapparts->UI_Draw(&(this->chara), (TPSparts->key_TPS.on()) ? &this->chara[TPSparts->sel_cam] : this->Get_Mine());
		}
		void BG_Draw(void) noexcept override {
			//map
			(*MAPPTs)->sky_Draw();
		}
		void Shadow_Draw_Far(void) noexcept override {
			//map
			(*MAPPTs)->Shadow_Draw_Far();
		}
		void Shadow_Draw_NearFar(void) noexcept override {
			//�L����
			for (auto& c : this->chara) {
				if ((c->get_pos() - GetCameraPosition()).size() > 1.f) {
					c->Draw_chara(0);
				}
			}
		}
		void Shadow_Draw(void) noexcept override {
			//map
			(*MAPPTs)->item_Draw();
			//�L����
			for (auto& c : this->chara) {
				if ((c->get_pos() - GetCameraPosition()).size() <= 2.5f) {
					c->Draw_chara(1);
				}
			}
		}
		void Main_Draw(void) noexcept override {
			//map
			(*MAPPTs)->main_Draw();
			//������
			this->hit_obj_p.draw();
			this->hit_b_obj_p.draw();
			//�T�C�g
			(*this->Get_Mine())->Draw_reticle();
			//�L����
			for (auto& c : this->chara) { c->Draw_chara(); }
			//���[�U�[
			for (auto& c : this->chara) { c->Draw_LAM_Effect(&this->chara, light); }
			//�e�e
			SetFogEnable(FALSE);
			SetUseLighting(FALSE);
			for (auto& c : this->chara) { c->Draw_ammo(); }
			SetUseLighting(TRUE);
			SetFogEnable(TRUE);
			//
		}
		void Item_Draw(void) noexcept override {
			lens_zoom = (*this->Get_Mine())->Draw_reticle_UI();
			lens_size = (*this->Get_Mine())->Get_reticle_size();
			if (lens_zoom > 1.0f) {
				use_lens = (*this->Get_Mine())->ads_on();
			}
			for (auto& c : this->chara) { c->Set_Draw_bullet(); }
			UIparts->item_Draw(&this->chara, this->Get_Mine());
		}
		void LAST_Draw(void) noexcept override {
			//TPS���_
			if (TPSparts->key_TPS.on()) {
				Hostpassparts_TPS->MAIN_draw();
			}
			//minimap
			minimapparts->Draw();
		}
	};
};