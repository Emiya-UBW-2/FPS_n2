#pragma once
class Sceneclass : Mainclass {
private:
public:
	class LOAD;
	class SELECT;
	class MAINLOOP;
	//
	class LOAD {
	private:
		//
		bool use_VR = false;
		//sound
		SoundHandle decision;
		SoundHandle cancel;
		SoundHandle cursor;
		//キー
		switchs left;
		switchs right;
		switchs Start;
		//データ
		GUNPARTs* temp_p = nullptr;
		PLAYERclass::Chara* mine_ptr = nullptr;
		int sel_p = 0;
		int sel_max = 0;
		std::vector<save_c> save_parts;
		std::vector<std::string> save_presets;
		//そのまま
		std::unique_ptr<UIclass::UI_LOAD, std::default_delete<UIclass::UI_LOAD>> UIparts;
		//引き継ぐ
		std::unique_ptr<MAINLOOP, std::default_delete<MAINLOOP>>* MAINLOOPscene;
	public:
		//
		LOAD(const bool& use_vr, std::unique_ptr<MAINLOOP, std::default_delete<MAINLOOP>>* MAINLOOPscene_t) {
			MAINLOOPscene = MAINLOOPscene_t;
			use_VR = use_vr;
			this->decision = SoundHandle::Load("data/audio/shot_2.wav");//
			this->cancel = SoundHandle::Load("data/audio/cancel.wav");
			this->cursor = SoundHandle::Load("data/audio/cursor.wav");
			UIparts = std::make_unique<UIclass::UI_LOAD>();
		}
		void Set() {
			{
				left.ready(false);
				right.ready(false);
				Start.ready(false);
			}
			mine_ptr = &(*MAINLOOPscene)->Get_Mine();

			SetUseMaskScreenFlag(FALSE);//←カスタム画面でエフェクトが出なくなるため入れる
			SetMousePoint(deskx / 2, desky / 2);											//
			{
				save_presets.clear();
				WIN32_FIND_DATA win32fdt;
				HANDLE hFind;
				hFind = FindFirstFile("data/save/*", &win32fdt);
				if (hFind != INVALID_HANDLE_VALUE) {
					do {
						//if ((win32fdt.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (win32fdt.cFileName[0] != '.')) {}
						if ((win32fdt.cFileName[0] != '.')) {
							save_presets.resize(save_presets.size() + 1);
							save_presets.back() = win32fdt.cFileName;
						}
					} while (FindNextFile(hFind, &win32fdt));
				} //else{ return false; }
				FindClose(hFind);
				sel_max = int(save_presets.size());
			}
			/*パーツデータをロード*/
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
					this->save_parts.resize(this->save_parts.size() + 1);
					this->save_parts.back() = savetmp;
				}
				file.close();
			}
		}
		bool UpDate() {
			{
				bool changef = false;
				//演算
				{
					{
						left.get_in(mine_ptr->get_akey());
						right.get_in(mine_ptr->get_dkey());
						Start.get_in(mine_ptr->get_jamp());
						if (left.push()) {
							sel_p--;
							changef = true;
						}
						if (right.push()) {
							sel_p++;
							changef = true;
						}
						if (sel_p < 0) { sel_p = sel_max - 1; }
						if (sel_p > sel_max - 1) { sel_p = 0; }

						if (changef) {
							/*パーツデータをロード*/
							{
								std::fstream file;
								save_parts.clear();
								file.open(("data/save/" + save_presets[sel_p]).c_str(), std::ios::binary | std::ios::in);
								save_c savetmp;
								while (true) {
									file.read((char*)&savetmp, sizeof(savetmp));
									if (file.eof()) {
										break;
									}
									this->save_parts.resize(this->save_parts.size() + 1);
									this->save_parts.back() = savetmp;
								}
								file.close();
							}
							//
						}
					}
					//
				}
				//campos,camvec,camupの指定
				{}
			}
			if (Start.push()) {
				return false;
			}
			return true;
			//
		}
		void Dispose(std::string* preset) {
			*preset = save_presets[sel_p];
		}
		void UI_Draw() {
			UIparts->UI_Draw(*MAINLOOPscene, save_parts, save_presets[sel_p], use_VR);
		}
		void BG_Draw(void) {
		}
		void Shadow_Draw(void) {
		}
		void Main_Draw(void) {
		}
	};
	//
	class SELECT {
	private:
		bool use_VR = false;
		//sound
		SoundHandle assemble;
		SoundHandle shot_se;
		SoundHandle slide_se;
		SoundHandle trigger_se;
		//sound
		SoundHandle decision;
		SoundHandle cancel;
		SoundHandle cursor;
		//カメラ
		cam_info camera_main;
		float fov_base = DX_PI_F / 2;
		//データ
		switchs up;
		switchs down;
		switchs left;
		switchs right;
		switchs shot;
		switchs Start;
		GUNPARTs* parts_p = nullptr;
		size_t parts_cat = SIZE_MAX;
		size_t port_cat = SIZE_MAX;
		PLAYERclass::Chara::g_parts* port_ptr = nullptr;
		std::vector<PLAYERclass::Chara::g_parts*> sight_ptr;
		int sel_p = 0;
		int chang = 0;
		int sel_max = 0;
		int chang_max = 0;
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
		PLAYERclass::Chara* mine_ptr = nullptr;
		//
		VECTOR_ref Shadow_minpos;
		VECTOR_ref Shadow_maxpos;
		VECTOR_ref Light_vec;
		COLOR_F Light_color = GetColorF(0, 0, 0, 0);
		//
		float change_per = 1.f;
		//
		std::vector<save_c> save_parts;
		std::string save_tgt = "1";
		//そのまま
		std::unique_ptr<UIclass::UI_CUSTOM, std::default_delete<UIclass::UI_CUSTOM>> UIparts;
		//引き継ぐ
		std::unique_ptr<DXDraw, std::default_delete<DXDraw>>* DrawPts;
		std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>* MAPPTs;
		std::unique_ptr<OPTION, std::default_delete<OPTION>>* OPTPTs;
		std::unique_ptr<MAINLOOP, std::default_delete<MAINLOOP>>* MAINLOOPscene;
	public:
		std::string preset;
	private:
		//必須品
		void Set_chang_haveto(std::vector<GUNPARTs>&data, const size_t& pts_cat, const size_t& pot_cat, int& chang_t) {
			//mine_ptr->Detach_child(m);
			mine_ptr->Detach_parts(pts_cat);
			chang_max = (int)data.size();
			chang_t = std::clamp(chang_t, 0, chang_max - 1);
			parts_p = &data[chang_t];

			mine_ptr->Attach_parts(parts_p, pts_cat, port_ptr, pot_cat);

			viewparts_buf = port_ptr->obj.frame(port_ptr->rail_f[pot_cat][0].first);
		}
		//非必須品
		void Set_chang_needs(std::vector<GUNPARTs>&data, const size_t& pts_cat, const size_t& pot_cat, int& chang_t, const int& sel = 0) {
			chang_max = (int)data.size() + 1;
			chang_t = std::clamp(chang_t, 0, chang_max - 1);
			if (chang_t == 0) { parts_p = nullptr; }
			else { parts_p = &data[std::max(chang_t - 1, 0)]; }
			viewparts_buf = port_ptr->obj.frame(port_ptr->rail_f[pot_cat][0].first);
			if (sel == 0) {
				mine_ptr->Detach_parts(pts_cat);
			}
			else {
				mine_ptr->Detach_parts(pts_cat, sel - 1);
			}
			if (chang_t != 0) {
				mine_ptr->Attach_parts(parts_p, pts_cat, port_ptr, pot_cat, sel);
			}
		}
		//非必須品3
		void Set_chang_need3(std::vector<GUNPARTs>&data1, std::vector<GUNPARTs>&data2, const size_t& pts_cat, const size_t& pot_cat, int& chang_t) {
			chang_max = (int)(data1.size() + 1 + data2.size());
			chang_t = std::clamp(chang_t, 0, chang_max - 1);
			if (chang_t == 0) { parts_p = nullptr; }
			else {
				if (chang_t < data1.size() + 1) { parts_p = &data1[std::max(chang_t - 1, 0)]; }
				else { parts_p = &data2[std::max(chang_t - 1, 0) - data1.size()]; }
			}
			mine_ptr->Detach_parts(pts_cat);
			if (chang_t != 0) {
				mine_ptr->Attach_parts(parts_p, pts_cat, port_ptr, pot_cat);
			}
			viewparts_buf = port_ptr->obj.frame(port_ptr->rail_f[pot_cat][0].first);
		}
		//サイトを乗せるかいなか
		bool Set_sight_at(PLAYERclass::Chara::g_parts* ports_ptr) {
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
	public:
		//
		auto& get_Shadow_minpos() { return Shadow_minpos; }
		auto& get_Shadow_maxpos() { return Shadow_maxpos; }
		auto& get_Light_vec() { return Light_vec; }
		auto& get_Light_color() { return Light_color; }
		auto& Get_Camera(void) { return camera_main; }
		//
		SELECT(std::unique_ptr<DXDraw, std::default_delete<DXDraw>>* DrawPts_t, std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>* MAPPTs_t, std::unique_ptr<OPTION, std::default_delete<OPTION>>* OPTPTs_t, std::unique_ptr<MAINLOOP, std::default_delete<MAINLOOP>>* MAINLOOPscene_t) {
			OPTPTs = OPTPTs_t;
			MAPPTs = MAPPTs_t;
			DrawPts = DrawPts_t;
			MAINLOOPscene = MAINLOOPscene_t;
			use_VR = (*DrawPts)->use_vr;
			this->assemble = SoundHandle::Load("data/audio/assemble.wav");
			this->decision = SoundHandle::Load("data/audio/shot_2.wav");//
			this->cancel = SoundHandle::Load("data/audio/cancel.wav");
			this->cursor = SoundHandle::Load("data/audio/cursor.wav");
			UIparts = std::make_unique<UIclass::UI_CUSTOM>();
		}
		void Set() {
			{
				up.ready(false);
				down.ready(false);
				left.ready(false);
				right.ready(false);
				shot.ready(false);
				Start.ready(false);
				Rot.ready(false);
				parts_p = nullptr;
				parts_cat = SIZE_MAX;
				sel_p = 0;
				chang = 0;
				sel_max = 0;
				chang_max = 0;
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
				mine_ptr = &(*MAINLOOPscene)->Get_Mine();
			}
			{
				SetCreateSoundTimeStretchRate(1.f / std::clamp(rate, 0.9f, 1.1f));
				shot_se = SoundHandle::Load(mine_ptr->get_audio().shot_path);
				slide_se = SoundHandle::Load(mine_ptr->get_audio().slide_path);
				trigger_se = SoundHandle::Load(mine_ptr->get_audio().trigger_path);
				SetCreateSoundTimeStretchRate(1.f);
			}
			SetUseMaskScreenFlag(FALSE);//←カスタム画面でエフェクトが出なくなるため入れる
			SetMousePoint(deskx / 2, desky / 2);											//
			fov_base = deg2rad(use_VR ? 180 : (*OPTPTs)->Fov);
			camera_main.set_cam_info(fov_base, 0.1f, 200.f);//1P
			//ライティング
			Shadow_maxpos = VGet(1.f, 1.f, 1.f);
			Shadow_minpos = VGet(-1.f, -1.f, -1.f);
			Light_vec = VGet(0.5f, -0.5f, 0.5f);
			Light_color = GetColorF(0.42f, 0.41f, 0.40f, 0.0f);
			/*パーツデータをロード*/
			{
				std::fstream file;
				save_parts.clear();
				file.open(("data/save/" + preset).c_str(), std::ios::binary | std::ios::in);
				save_c savetmp;
				while (true) {
					file.read((char*)&savetmp, sizeof(savetmp));
					if (file.eof()) {
						break;
					}
					this->save_parts.resize(this->save_parts.size() + 1);
					this->save_parts.back() = savetmp;
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
				//マズル
				save_parts.resize(save_parts.size() + 1);
				save_parts.back().cang_ = 0;
				save_parts.back().type_ = EnumGunParts::PARTS_MAZZULE;
				save_parts.back().pt_cat_ = EnumAttachPoint::MAZZULE_BASE;
				save_parts.back().pt_type_ = EnumGunParts::PARTS_BASE;
				save_parts.back().pt_sel_ = 0;
				//ダストカバー
				save_parts.resize(save_parts.size() + 1);
				save_parts.back().cang_ = 0;
				save_parts.back().type_ = EnumGunParts::PARTS_DUSTCOVER;
				save_parts.back().pt_cat_ = EnumAttachPoint::DUSTCOVER_BASE;
				save_parts.back().pt_type_ = EnumGunParts::PARTS_BASE;
				save_parts.back().pt_sel_ = 0;
				//ストック
				save_parts.resize(save_parts.size() + 1);
				save_parts.back().cang_ = 0;
				save_parts.back().type_ = EnumGunParts::PARTS_STOCK;
				save_parts.back().pt_cat_ = EnumAttachPoint::STOCK_BASE;
				save_parts.back().pt_type_ = EnumGunParts::PARTS_BASE;
				save_parts.back().pt_sel_ = 0;
			}
			//*/
			//読み込み
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
							temp_ptr = &mine_ptr->base;
							break;
						case EnumGunParts::PARTS_UPER_HGUARD:
							temp_ptr = mine_ptr->get_parts(tmp_save.pt_type_);
							break;
						case EnumGunParts::PARTS_UNDER_HGUARD:
							temp_ptr = mine_ptr->get_parts(tmp_save.pt_type_);
							break;
						case EnumGunParts::PARTS_DUSTCOVER:
							temp_ptr = mine_ptr->get_parts(tmp_save.pt_type_);
							break;
						case EnumGunParts::PARTS_MOUNT:
							if (tmp_save.pt_sel_ != SIZE_MAX && tmp_save.pt_sel_ < mine_ptr->mount_.size()) {
								temp_ptr = &mine_ptr->mount_[tmp_save.pt_sel_];
							}
							break;
						default:
							break;
						}
					}
					if (tmp_save.type_ == EnumGunParts::PARTS_SIGHT) {
						pppp++;
					}
					mine_ptr->Attach_parts(temp_p, tmp_save.type_, temp_ptr, tmp_save.pt_cat_, pppp);
				}
				//save_parts.clear();
			}
		}
		bool UpDate() {
			{
				bool changef = false;
				//演算
				{
					{
						mine_ptr->set_gun_pos(VGet(0, 0, 0), MGetIdent());
						mine_ptr->Set_gun();
						mine_ptr->set_mag_pos();
						mine_ptr->Set_mag();

						up.get_in(mine_ptr->get_wkey());
						down.get_in(mine_ptr->get_skey());
						left.get_in(mine_ptr->get_akey());
						right.get_in(mine_ptr->get_dkey());
						shot.get_in(mine_ptr->get_shoot());
						Start.get_in(mine_ptr->get_jamp());
						Rot.get_in(mine_ptr->get_selecter());
						if (Start_b) {
							Start_b = false;
							changef = true;
							int pp = mine_ptr->base.thisparts->Select_Chose(1);
							if (pp != -1) {
								mine_ptr->gun_stat_now->get_select() = uint8_t(pp);
							}
						}
						if (left.push()) {
							sel_p--;
							chang = 0;
							changef = true;
						}
						if (right.push()) {
							sel_p++;
							chang = 0;
							changef = true;
						}
						if (sel_p < 0) { sel_p = sel_max - 1; }
						if (sel_p > sel_max - 1) { sel_p = 0; }

						if (up.push()) {
							chang--;
							changef = true;
						}
						if (down.push()) {
							chang++;
							changef = true;
						}
						if (chang < 0) { chang = chang_max - 1; }
						if (chang > chang_max - 1) { chang = 0; }
					}
					if (changef) {
						Rot.first = false;
						port_ptr = nullptr;
						sel_max = 0;
						//magazine
						{
							if (sel_p == sel_max) {
								port_ptr = &mine_ptr->base;
								port_cat = EnumAttachPoint::MAGAZINE_BASE;
								parts_cat = EnumGunParts::PARTS_MAGAZINE;
								Set_chang_haveto(*(*MAINLOOPscene)->get_parts_data(parts_cat), parts_cat, port_cat, chang);
								{
									if (save_parts.size() < size_t(sel_max) + 1) {
										save_parts.resize(size_t(sel_max) + 1);
									}
									save_parts[sel_max].cang_ = chang;
									save_parts[sel_max].type_ = parts_cat;
									save_parts[sel_max].pt_cat_ = port_cat;
									save_parts[sel_max].pt_type_ = EnumGunParts::PARTS_BASE;
									save_parts[sel_max].pt_sel_ = 0;
								}
							}
							sel_max++;
						}
						//grip
						{
							if (sel_p == sel_max) {
								port_ptr = &mine_ptr->base;
								port_cat = EnumAttachPoint::GRIP_BASE;
								parts_cat = EnumGunParts::PARTS_GRIP;
								Set_chang_haveto(*(*MAINLOOPscene)->get_parts_data(parts_cat), parts_cat, port_cat, chang);
								{
									if (save_parts.size() < size_t(sel_max) + 1) {
										save_parts.resize(size_t(sel_max) + 1);
									}
									save_parts[sel_max].cang_ = chang;
									save_parts[sel_max].type_ = parts_cat;
									save_parts[sel_max].pt_cat_ = port_cat;
									save_parts[sel_max].pt_type_ = EnumGunParts::PARTS_BASE;
									save_parts[sel_max].pt_sel_ = 0;
								}
							}
							sel_max++;
						}
						//uperhandguard
						{
							if (sel_p == sel_max) {
								port_ptr = &mine_ptr->base;
								port_cat = EnumAttachPoint::UPER_HANDGUARD;
								parts_cat = EnumGunParts::PARTS_UPER_HGUARD;
								Set_chang_haveto(*(*MAINLOOPscene)->get_parts_data(parts_cat), parts_cat, port_cat, chang);
								//
								{
									if (save_parts.size() < size_t(sel_max) + 1) {
										save_parts.resize(size_t(sel_max) + 1);
									}
									save_parts[sel_max].cang_ = chang;
									save_parts[sel_max].type_ = parts_cat;
									save_parts[sel_max].pt_cat_ = port_cat;
									save_parts[sel_max].pt_type_ = EnumGunParts::PARTS_BASE;
									save_parts[sel_max].pt_sel_ = 0;
								}
							}
							sel_max++;
						}
						//underhandguard
						{
							if (sel_p == sel_max) {
								port_ptr = &mine_ptr->base;
								port_cat = EnumAttachPoint::UNDER_HANDGUARD;
								parts_cat = EnumGunParts::PARTS_UNDER_HGUARD;
								Set_chang_haveto(*(*MAINLOOPscene)->get_parts_data(parts_cat), parts_cat, port_cat, chang);
								{
									if (save_parts.size() < size_t(sel_max) + 1) {
										save_parts.resize(size_t(sel_max) + 1);
									}
									save_parts[sel_max].cang_ = chang;
									save_parts[sel_max].type_ = parts_cat;
									save_parts[sel_max].pt_cat_ = port_cat;
									save_parts[sel_max].pt_type_ = EnumGunParts::PARTS_BASE;
									save_parts[sel_max].pt_sel_ = 0;
								}
							}
							sel_max++;
						}
						//foregrip
						{
							port_ptr = mine_ptr->get_parts(EnumGunParts::PARTS_UNDER_HGUARD);
							port_cat = EnumAttachPoint::UNDER_RAIL;
							if (port_ptr->rail_f[port_cat][0].first > 0) {
								if (sel_p == sel_max) {
									parts_cat = EnumGunParts::PARTS_FOREGRIP;
									Set_chang_needs(*(*MAINLOOPscene)->get_parts_data(parts_cat), parts_cat, port_cat, chang);
									{
										if (save_parts.size() < size_t(sel_max) + 1) {
											save_parts.resize(size_t(sel_max) + 1);
										}
										save_parts[sel_max].cang_ = size_t(chang) - 1;
										save_parts[sel_max].type_ = parts_cat;
										save_parts[sel_max].pt_cat_ = port_cat;
										save_parts[sel_max].pt_type_ = EnumGunParts::PARTS_UNDER_HGUARD;
										save_parts[sel_max].pt_sel_ = 0;
									}
								}
								sel_max++;
							}
						}
						//light/laser
						{
							port_ptr = mine_ptr->get_parts(EnumGunParts::PARTS_UNDER_HGUARD);
							port_cat = EnumAttachPoint::LEFTSIDE_RAIL;
							if (port_ptr->rail_f[port_cat][0].first > 0) {
								if (sel_p == sel_max) {
									if (chang == 0) {
										parts_cat = EnumGunParts::PARTS_LIGHT;
									}
									else {
										parts_cat = (chang < (*MAINLOOPscene)->get_parts_data(EnumGunParts::PARTS_LIGHT)->size() + 1) ? EnumGunParts::PARTS_LIGHT : EnumGunParts::PARTS_LASER;
									}
									mine_ptr->Detach_parts(EnumGunParts::PARTS_LIGHT);
									mine_ptr->Detach_parts(EnumGunParts::PARTS_LASER);
									Set_chang_need3(*(*MAINLOOPscene)->get_parts_data(EnumGunParts::PARTS_LIGHT), (*(*MAINLOOPscene)->get_parts_data(EnumGunParts::PARTS_LASER)), parts_cat, port_cat, chang);
									{
										auto chang_t = chang;
										if (chang != 0) {
											if (chang < (*MAINLOOPscene)->get_parts_data(EnumGunParts::PARTS_LIGHT)->size() + 1) { chang_t = std::max(chang - 1, 0); }
											else { chang_t = int(std::max(chang - 1, 0) - (*MAINLOOPscene)->get_parts_data(EnumGunParts::PARTS_LIGHT)->size()); }
										}

										if (save_parts.size() < size_t(sel_max) + 1) {
											save_parts.resize(size_t(sel_max) + 1);
										}
										save_parts[sel_max].cang_ = chang_t;
										save_parts[sel_max].type_ = parts_cat;
										save_parts[sel_max].pt_cat_ = port_cat;
										save_parts[sel_max].pt_type_ = EnumGunParts::PARTS_UNDER_HGUARD;
										save_parts[sel_max].pt_sel_ = 0;
									}
								}
								sel_max++;
							}
						}
						//sidemount
						{
							if (sel_p == sel_max) {
								port_ptr = &mine_ptr->base;
								port_cat = EnumAttachPoint::SIDEMOUNT_BASE;
								parts_cat = EnumGunParts::PARTS_MOUNT;
								{
									chang_max = (int)1 + 1; //chang_max = (int)mount_data.size();
									chang = std::clamp(chang, 0, chang_max - 1);
									if (chang == 0) { parts_p = nullptr; }
									else { parts_p = &(*(*MAINLOOPscene)->get_parts_data(parts_cat))[std::max(chang - 1, 0)]; }
									mine_ptr->Detach_parts(parts_cat, 0);
									if (chang != 0) {
										mine_ptr->Attach_parts(parts_p, parts_cat, port_ptr, port_cat);
									}
									viewparts_buf = port_ptr->obj.frame(port_ptr->rail_f[port_cat][0].first);
								}
								{
									if (save_parts.size() < size_t(sel_max) + 1) {
										save_parts.resize(size_t(sel_max) + 1);
									}
									save_parts[sel_max].cang_ = size_t(chang) - 1;
									save_parts[sel_max].type_ = parts_cat;
									save_parts[sel_max].pt_cat_ = port_cat;
									save_parts[sel_max].pt_type_ = EnumGunParts::PARTS_BASE;
									save_parts[sel_max].pt_sel_ = 0;
								}
							}
							sel_max++;
						}
						//sidemount_2
						{
							if (mine_ptr->mount_.size() > 0) {
								port_ptr = &mine_ptr->mount_[0];
								port_cat = EnumAttachPoint::SIDEMOUNT;
								if (port_ptr->rail_f[port_cat][0].first > 0) {
									if (sel_p == sel_max) {
										parts_cat = EnumGunParts::PARTS_MOUNT;
										{
											chang_max = (int)1 + 1;
											chang = std::clamp(chang, 0, chang_max - 1);
											if (chang == 0) { parts_p = nullptr; }
											else { parts_p = &(*(*MAINLOOPscene)->get_parts_data(parts_cat))[chang]; }
											if (mine_ptr->mount_.size() == 2) {
												mine_ptr->Detach_parts(parts_cat, 1);
											}
											if (chang != 0) {
												mine_ptr->Attach_parts(parts_p, parts_cat, port_ptr, port_cat);
											}
											viewparts_buf = port_ptr->obj.frame(port_ptr->rail_f[port_cat][0].first);
										}
										//
										{
											if (save_parts.size() < size_t(sel_max) + 1) {
												save_parts.resize(size_t(sel_max) + 1);
											}
											save_parts[sel_max].cang_ = chang;
											save_parts[sel_max].type_ = parts_cat;
											save_parts[sel_max].pt_cat_ = port_cat;
											save_parts[sel_max].pt_type_ = EnumGunParts::PARTS_MOUNT;
											save_parts[sel_max].pt_sel_ = 0;
										}
									}
									sel_max++;
								}
							}
						}
						//マズル
						{
							if (sel_p == sel_max) {
								port_ptr = &mine_ptr->base;
								port_cat = EnumAttachPoint::MAZZULE_BASE;
								parts_cat = EnumGunParts::PARTS_MAZZULE;
								Set_chang_needs(*(*MAINLOOPscene)->get_parts_data(parts_cat), parts_cat, port_cat, chang);
								{
									if (save_parts.size() < size_t(sel_max) + 1) {
										save_parts.resize(size_t(sel_max) + 1);
									}
									save_parts[sel_max].cang_ = size_t(chang) - 1;
									save_parts[sel_max].type_ = parts_cat;
									save_parts[sel_max].pt_cat_ = port_cat;
									save_parts[sel_max].pt_type_ = EnumGunParts::PARTS_BASE;
									save_parts[sel_max].pt_sel_ = 0;
								}
							}
							sel_max++;
						}
						//ダストカバー
						{
							if (sel_p == sel_max) {
								port_ptr = &mine_ptr->base;
								port_cat = EnumAttachPoint::DUSTCOVER_BASE;
								parts_cat = EnumGunParts::PARTS_DUSTCOVER;
								Set_chang_needs(*(*MAINLOOPscene)->get_parts_data(parts_cat), parts_cat, port_cat, chang);
								//
								{
									if (save_parts.size() < size_t(sel_max) + 1) {
										save_parts.resize(size_t(sel_max) + 1);
									}
									save_parts[sel_max].cang_ = size_t(chang) - 1;
									save_parts[sel_max].type_ = parts_cat;
									save_parts[sel_max].pt_cat_ = port_cat;
									save_parts[sel_max].pt_type_ = EnumGunParts::PARTS_BASE;
									save_parts[sel_max].pt_sel_ = 0;
								}
							}
							sel_max++;
						}
						//ストック
						{
							if (sel_p == sel_max) {
								port_ptr = &mine_ptr->base;
								port_cat = EnumAttachPoint::STOCK_BASE;
								parts_cat = EnumGunParts::PARTS_STOCK;
								Set_chang_needs(*(*MAINLOOPscene)->get_parts_data(parts_cat), parts_cat, port_cat, chang);
								{
									if (save_parts.size() < size_t(sel_max) + 1) {
										save_parts.resize(size_t(sel_max) + 1);
									}
									save_parts[sel_max].cang_ = size_t(chang) - 1;
									save_parts[sel_max].type_ = parts_cat;
									save_parts[sel_max].pt_cat_ = port_cat;
									save_parts[sel_max].pt_type_ = EnumGunParts::PARTS_BASE;
									save_parts[sel_max].pt_sel_ = 0;
								}
							}
							sel_max++;
						}
						//サイト判定
						{
							sight_ptr.resize(5);
							for (auto& s : sight_ptr) {
								s = nullptr;
							}
							if (Set_sight_at(mine_ptr->get_parts(EnumGunParts::PARTS_UPER_HGUARD))) {
								sight_ptr[0] = mine_ptr->get_parts(EnumGunParts::PARTS_UPER_HGUARD);
							}
							if (mine_ptr->mount_.size() > 1) {
								if (Set_sight_at(&mine_ptr->mount_[1])) {
									sight_ptr[1] = &mine_ptr->mount_[1];
								}
							}

							if (Set_sight_at(mine_ptr->get_parts(EnumGunParts::PARTS_DUSTCOVER))) {
								sight_ptr[2] = mine_ptr->get_parts(EnumGunParts::PARTS_DUSTCOVER);
							}

						}
						//sight
						{
							int ssp = sel_max;
							for (auto& s : sight_ptr) {
								if (s != nullptr) {
									port_cat = EnumAttachPoint::UPER_RAIL;
									if (s->rail_f[port_cat][0].first > 0) {
										if (sel_p == sel_max) {
											port_ptr = s;
											parts_cat = EnumGunParts::PARTS_SIGHT;
											Set_chang_needs(*(*MAINLOOPscene)->get_parts_data(parts_cat), parts_cat, port_cat, chang, (sel_max - ssp + 1));
											{
												if (save_parts.size() < size_t(sel_max) + 1) {
													save_parts.resize(size_t(sel_max) + 1);
												}
												save_parts[sel_max].cang_ = size_t(chang) - 1;
												save_parts[sel_max].type_ = parts_cat;
												save_parts[sel_max].pt_cat_ = port_cat;

												save_parts[sel_max].pt_type_ = s->get_type();
												if (save_parts[sel_max].pt_type_ == EnumGunParts::PARTS_MOUNT) {
													save_parts[sel_max].pt_sel_ = 1;//改善
												}
											}
										}
										sel_max++;
									}
								}
							}
							//
						}
						//
						{
							change_per = 1.f;
							assemble.play(DX_PLAYTYPE_BACK, TRUE);

							xrad_t = viewparts_buf.y()*1000.f;
							yrad_t = ((viewparts_buf.x() > 0.f) ? 90.f - viewparts_buf.z()*100.f : -90.f + viewparts_buf.z()*100.f);
						}
						//
					}
					//
					easing_set(&mine_ptr->get_gunanime_trigger()->per, shot.press(), 0.5f);
					if (shot.push()) {
						if (!mine_ptr->get_gunf()) {
							mine_ptr->get_gunf() = true;
							//todo ディレイつける
							{
								shot_se.play(DX_PLAYTYPE_BACK, TRUE);
								slide_se.play(DX_PLAYTYPE_BACK, TRUE);
								trigger_se.play(DX_PLAYTYPE_BACK, TRUE);
								//薬莢
								mine_ptr->calc_cart();
							}
							//エフェクト
							mine_ptr->calc_shot_effect();
						}
					}
					//薬莢
					mine_ptr->calc_cart_every(rate);
					//
					mine_ptr->Set_select();
					mine_ptr->Set_shot_anime(rate, true);
					mine_ptr->base.obj.work_anime();
					//薬莢の処理
					mine_ptr->update_cart();
					//
					mine_ptr->update_effect();
				}
				//campos,camvec,camupの指定
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
					{
						auto tmp = viewparts;
						if (std::abs(yrad_t) > 90) {
							if (mine_ptr->get_parts(EnumGunParts::PARTS_MAZZULE)->get_mazzuletype() > 0) {
								tmp = VECTOR_ref(mine_ptr->get_parts(EnumGunParts::PARTS_MAZZULE)->mazzule_pos()) - tmp;
							}
							else {
								tmp = VECTOR_ref(mine_ptr->base.mazzule_pos()) - tmp;
							}
						}
						else {
							tmp = VECTOR_ref(VGet(0, 0, 0.5f)) - tmp;//todo
						}
						range_tgt = std::hypotf(sin(deg2rad(yrad_t))*0.25f, cos(deg2rad(yrad_t))*(std::abs(tmp.z()) + camera_main.near_ * 2.f));
					}
					range_t = (Rot.on() && !changef) ? std::clamp(range_t - float(GetMouseWheelRotVol())*0.1f, range_tgt, 5.f) : range_tgt;

					easing_set(&camparts, VGet(
						range_t*cos(deg2rad(xrad_t))*sin(deg2rad(yrad_t)),
						range_t*sin(deg2rad(xrad_t)),
						range_t*cos(deg2rad(xrad_t))*cos(deg2rad(yrad_t))),
						0.8f
					);
					camera_main.camvec = viewparts;
					camera_main.campos = camera_main.camvec + camparts;
					camera_main.camup = VGet(0, 1.f, 0);
				}
			}
			if (Start.push()) {
				return false;
			}
			return true;
			//
		}
		void Dispose(void) {
			//データセーブ
			{
				std::fstream file;
				file.open(("data/save/" + save_tgt + ".dat").c_str(), std::ios::binary | std::ios::out);
				for (auto&tmp_save : save_parts) {
					file.write((char*)&tmp_save, sizeof(tmp_save));
				}
				file.close();
			}
			mine_ptr->gun_stat_now->get_select() = 0;
			shot_se.Dispose();
			slide_se.Dispose();
			trigger_se.Dispose();
		}
		void UI_Draw() {
			UIparts->UI_Draw(*MAINLOOPscene, parts_cat, Rot, mine_ptr, parts_p, change_per, use_VR);
			easing_set(&change_per, 0.f, 0.5f);
		}
		void BG_Draw(void) {
			DrawBox(0, 0, 1920, 1080, GetColor(192, 192, 192), TRUE);
		}
		void Shadow_Draw_Far() {

		}
		void Shadow_Draw(void) {
			mine_ptr->Draw_gun();
			mine_ptr->get_parts(EnumGunParts::PARTS_MAGAZINE)->Draw(2);
		}
		void Main_Draw(void) {
			mine_ptr->Draw_gun();
			mine_ptr->get_parts(EnumGunParts::PARTS_MAGAZINE)->Draw(2);
		}
		void Item_Draw(void) {
		}
		void LAST_Draw(void) {
		}
	};
	//
	class MAINLOOP {
		//TPS操作
		class TPS_parts {
			std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>* MAPPTs;
		public:
			switchs key_TPS;
			float xr_cam = 0.f;
			float yr_cam = 0.f;
			int sel_cam = 0;
			//カメラ
			cam_info camera_TPS;

			TPS_parts(std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>* MAPPTs_t) {
				MAPPTs = MAPPTs_t;
			}
			void Set(float& fov_pc) {
				this->key_TPS.ready(false);
				this->camera_TPS.campos = VGet(0, 1.8f, -10);
				this->camera_TPS.set_cam_info(deg2rad(fov_pc), 0.1f, 200.f);
			}
			void Set_info(std::vector<PLAYERclass::Chara>&chara) {
				if (this->key_TPS.on()) {
					{
						const auto fps_ = GetFPS();
						//cam
						for (int i = 0; i < std::min<size_t>(chara.size(), 10); i++) {
							if (CheckHitKey(KEY_INPUT_1 + i) != 0) {
								this->sel_cam = i;
							}
						}
						//pos
						{
							if (CheckHitKey(KEY_INPUT_LEFT) != 0) {
								this->camera_TPS.campos.x(this->camera_TPS.campos.x() - 10.f / fps_ * cos(this->yr_cam));
								this->camera_TPS.campos.z(this->camera_TPS.campos.z() + 10.f / fps_ * sin(this->yr_cam));
							}
							if (CheckHitKey(KEY_INPUT_RIGHT) != 0) {
								this->camera_TPS.campos.x(this->camera_TPS.campos.x() + 10.f / fps_ * cos(this->yr_cam));
								this->camera_TPS.campos.z(this->camera_TPS.campos.z() - 10.f / fps_ * sin(this->yr_cam));
							}
							if (CheckHitKey(KEY_INPUT_UP) != 0) {
								this->camera_TPS.campos.z(this->camera_TPS.campos.z() + 10.f / fps_ * cos(this->yr_cam));
								this->camera_TPS.campos.x(this->camera_TPS.campos.x() + 10.f / fps_ * sin(this->yr_cam));
							}
							if (CheckHitKey(KEY_INPUT_DOWN) != 0) {
								this->camera_TPS.campos.z(this->camera_TPS.campos.z() - 10.f / fps_ * cos(this->yr_cam));
								this->camera_TPS.campos.x(this->camera_TPS.campos.x() - 10.f / fps_ * sin(this->yr_cam));
							}
							this->camera_TPS.campos.x(std::clamp(this->camera_TPS.campos.x(), (*MAPPTs)->map_col_get().mesh_minpos(0).x(), (*MAPPTs)->map_col_get().mesh_maxpos(0).x()));
							this->camera_TPS.campos.z(std::clamp(this->camera_TPS.campos.z(), (*MAPPTs)->map_col_get().mesh_minpos(0).z(), (*MAPPTs)->map_col_get().mesh_maxpos(0).z()));
						}
						//rad
						{
							VECTOR_ref vec_2 = (chara[this->sel_cam].get_head_pos() - this->camera_TPS.campos).Norm();
							VECTOR_ref vec_z = (this->camera_TPS.camvec - this->camera_TPS.campos).Norm();
							VECTOR_ref vec_x = vec_z.cross(this->camera_TPS.camup);

							this->xr_cam -= deg2rad(int(vec_z.cross(vec_2).dot(vec_x) * 50))*0.1f;
							this->yr_cam -= deg2rad(int(vec_x.dot(vec_2) * 50))*0.1f;
							this->xr_cam = std::clamp(this->xr_cam, deg2rad(-89), deg2rad(89));
						}
						//
						this->camera_TPS.camvec = this->camera_TPS.campos + MATRIX_ref::Vtrans(VGet(0, 0, 1), MATRIX_ref::RotX(this->xr_cam)*MATRIX_ref::RotY(this->yr_cam));
						this->camera_TPS.camup = VGet(0, 1.f, 0);
					}
				}
			}
		};
		//カメラ
		cam_info camera_main;
		float fov_base = DX_PI_F / 2;
		bool use_VR = false;
		//データ
		MV1 body_obj, body_obj_lag, body_col;		//身体モデル
		MV1 hit_pic;								//弾痕
		GraphHandle light;							//ライトテクスチャ
		std::vector<GUNPARTs> mazzule_data;			//GUNデータ
		std::vector<GUNPARTs> grip_data;			//GUNデータ
		std::vector<GUNPARTs> uperhandguard_data;	//GUNデータ
		std::vector<GUNPARTs> underhandguard_data;	//GUNデータ
		std::vector<GUNPARTs> stock_data;			//GUNデータ
		std::vector<GUNPARTs> dustcover_data;		//GUNデータ
		std::vector<GUNPARTs> sight_data;			//GUNデータ
		std::vector<GUNPARTs> foregrip_data;		//GUNデータ
		std::vector<GUNPARTs> light_data;			//GUNデータ
		std::vector<GUNPARTs> laser_data;			//GUNデータ
		std::vector<GUNPARTs> mount_data;			//GUNデータ
		std::vector<GUNPARTs> gun_data;				//GUNデータ
		std::vector<GUNPARTs> magazine_data;		//GUNデータ
		std::vector<Meds> meds_data;				//GUNデータ
		//オブジェ
		std::vector<PLAYERclass::Chara> chara;					//キャラ
		//std::vector<Items> item;					//アイテム
		std::vector<Hit> hit_obj;					//弾痕
		size_t hit_buf = 0;							//
		//そのまま
		std::unique_ptr<HostPassEffect, std::default_delete<HostPassEffect>> Hostpassparts_TPS;
		std::unique_ptr<TPS_parts, std::default_delete<TPS_parts>> TPSparts;
		std::unique_ptr<UIclass::UI_MAINLOOP, std::default_delete<UIclass::UI_MAINLOOP>> UIparts;
		std::unique_ptr<RULE_parts, std::default_delete<RULE_parts>> RULEparts;
		std::unique_ptr<MAPclass::MiniMap, std::default_delete<MAPclass::MiniMap>> minimapparts;
		//引き継ぐ
		std::unique_ptr<DXDraw, std::default_delete<DXDraw>>* DrawPts;
		std::unique_ptr<OPTION, std::default_delete<OPTION>>* OPTPTs;
		std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>* MAPPTs;
		//
		VECTOR_ref Shadow_minpos;
		VECTOR_ref Shadow_maxpos;
		VECTOR_ref Light_vec;
		COLOR_F Light_color = GetColorF(0, 0, 0, 0);
	public:
		PLAYERclass::Chara& Get_Mine(void) { return this->chara[0]; }
		std::vector<GUNPARTs>* get_parts_data(const size_t& type_sel) {
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
			case EnumGunParts::PARTS_LIGHT:
				return &this->light_data;
			case EnumGunParts::PARTS_LASER:
				return &this->laser_data;
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
		std::vector<Meds>& get_meds_data(void) { return this->meds_data; }
	public:
		auto& get_Shadow_minpos() { return Shadow_minpos; }
		auto& get_Shadow_maxpos() { return Shadow_maxpos; }
		auto& get_Light_vec() { return Light_vec; }
		auto& get_Light_color() { return Light_color; }
		auto& Get_Camera(void) { return this->camera_main; }
		MAINLOOP(std::unique_ptr<DXDraw, std::default_delete<DXDraw>>* DrawPts_t, std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>* MAPPTs_t, std::unique_ptr<OPTION, std::default_delete<OPTION>>* OPTPTs_t) {
			//
			DrawPts = DrawPts_t;
			MAPPTs = MAPPTs_t;
			OPTPTs = OPTPTs_t;

			use_VR = (*DrawPts)->use_vr;
			//
			UIparts = std::make_unique<UIclass::UI_MAINLOOP>(MAPPTs, &RULEparts);
			TPSparts = std::make_unique<TPS_parts>(MAPPTs);
			RULEparts = std::make_unique<RULE_parts>();
			Hostpassparts_TPS = std::make_unique<HostPassEffect>((*OPTPTs)->DoF, (*OPTPTs)->Bloom, (*OPTPTs)->SSAO, deskx, desky);	//ホストパスエフェクト(フルスクリーン向け、TPS用)
			minimapparts = std::make_unique<MAPclass::MiniMap>(MAPPTs);															//ミニマップ
			{
				//model
				light = GraphHandle::Load("data/light.png");					//ライト
				MV1::Load("data/model/body2/model.mv1", &this->body_obj, true);	//身体
				MV1SetLoadModelPhysicsWorldGravity(-1.f);
				MV1::Load("data/model/body_lag/model.mv1", &this->body_obj_lag, true);	//身体
				MV1::Load("data/model/body2/col.mv1", &this->body_col, true);	//身体col
				MV1::Load("data/model/hit/model.mv1", &this->hit_pic, true);	//弾痕
				//
				this->mazzule_data.resize(2);
				this->mazzule_data[0].mod.Ready("data/parts/mazzule/", "00_AK_6P1_Compensator");
				this->mazzule_data[1].mod.Ready("data/parts/mazzule/", "01_sup");
				this->grip_data.resize(3);
				this->grip_data[0].mod.Ready("data/parts/grip/", "00_AK_6P1_Wood_Grip");
				this->grip_data[1].mod.Ready("data/parts/grip/", "01_AK_original_Grip");
				this->grip_data[2].mod.Ready("data/parts/grip/", "02_AK_Zenit_RK3_Grip");
				this->uperhandguard_data.resize(5);
				this->uperhandguard_data[0].mod.Ready("data/parts/uper_handguard/", "00_AK_6P1_Uper_Handguard");
				this->uperhandguard_data[1].mod.Ready("data/parts/uper_handguard/", "01_AK_Original_Polymer_Uper_Handguard");
				this->uperhandguard_data[2].mod.Ready("data/parts/uper_handguard/", "02_AK_Steel_Uper_Handguard");
				this->uperhandguard_data[3].mod.Ready("data/parts/uper_handguard/", "03_AK_Rail_Band");
				this->uperhandguard_data[4].mod.Ready("data/parts/uper_handguard/", "04_AK_6P20_Uper_Handguard");
				this->underhandguard_data.resize(4);
				this->underhandguard_data[0].mod.Ready("data/parts/under_handguard/", "00_AK_6P1_Under_Handguard");
				this->underhandguard_data[1].mod.Ready("data/parts/under_handguard/", "01_AK_Original_Polymer_Under_Handguard");
				this->underhandguard_data[2].mod.Ready("data/parts/under_handguard/", "02_AK_Zenit_B10M_Under_Handguard");
				this->underhandguard_data[3].mod.Ready("data/parts/under_handguard/", "03_AK_6P20_Under_Handguard");
				this->mount_data.resize(2);
				this->mount_data[0].mod.Ready("data/parts/mount/", "00_AK_Dovetail_Mount");
				this->mount_data[1].mod.Ready("data/parts/mount/", "01_AK_SideRailMount");
				this->stock_data.resize(2);
				this->stock_data[0].mod.Ready("data/parts/stock/", "00_AK_6P1_Stock");
				this->stock_data[1].mod.Ready("data/parts/stock/", "00_AK_6P20_Stock");
				this->dustcover_data.resize(2);
				this->dustcover_data[0].mod.Ready("data/parts/dustcover/", "00_AK_6P1_Dustcover");
				this->dustcover_data[1].mod.Ready("data/parts/dustcover/", "01_AK_DogLegRail_Gen3_dustcover");
				this->sight_data.resize(2);
				this->sight_data[0].mod.Ready("data/parts/sight/", "00_Eotech_XPS3_HoloSight");
				this->sight_data[1].mod.Ready("data/parts/sight/", "01_AIMPOINT_T1_DotSight");
				this->foregrip_data.resize(2);
				this->foregrip_data[0].mod.Ready("data/parts/foregrip/", "00_Standard_Vertical_Foregrip");
				this->foregrip_data[1].mod.Ready("data/parts/foregrip/", "01_Bipod_Vertical_Foregrip");
				this->light_data.resize(1);
				this->light_data[0].mod.Ready("data/parts/light/", "00_Tactical_Flashlight");
				this->laser_data.resize(1);
				this->laser_data[0].mod.Ready("data/parts/laser/", "00_NcSTAR_Laser");
				this->gun_data.resize(1);
				this->gun_data[0].mod.Ready("data/gun/", "00_AKM");
				//MAGデータ
				this->magazine_data.resize(2);
				this->magazine_data[0].mod.Ready("data/mag/", "00_AK_55_Magazine");
				this->magazine_data[1].mod.Ready("data/mag/", "01_AK_55_Magazine");
				//MEDデータ
				this->meds_data.resize(1);
				this->meds_data[0].mod.Ready("data/medkit/", "AIDkit");
			}
		}
		void Start_After() {
			//GUNデータ2
			for (auto& g : this->mazzule_data) { g.Set_datas(&g - &this->mazzule_data[0], EnumGunParts::PARTS_MAZZULE); }
			for (auto& g : this->grip_data) { g.Set_datas(&g - &this->grip_data[0], EnumGunParts::PARTS_NONE); }
			for (auto& g : this->uperhandguard_data) { g.Set_datas(&g - &this->uperhandguard_data[0], EnumGunParts::PARTS_NONE); }
			for (auto& g : this->underhandguard_data) { g.Set_datas(&g - &this->underhandguard_data[0], EnumGunParts::PARTS_NONE); }
			for (auto& g : this->mount_data) { g.Set_datas(&g - &this->mount_data[0], EnumGunParts::PARTS_NONE); }
			for (auto& g : this->stock_data) { g.Set_datas(&g - &this->stock_data[0], EnumGunParts::PARTS_STOCK); }
			for (auto& g : this->dustcover_data) { g.Set_datas(&g - &this->dustcover_data[0], EnumGunParts::PARTS_NONE); }
			for (auto& g : this->light_data) { g.Set_datas(&g - &this->light_data[0], EnumGunParts::PARTS_NONE); }
			for (auto& g : this->laser_data) { g.Set_datas(&g - &this->laser_data[0], EnumGunParts::PARTS_NONE); }
			for (auto& g : this->foregrip_data) { g.Set_datas(&g - &this->foregrip_data[0], EnumGunParts::PARTS_NONE); }
			for (auto& g : this->sight_data) { g.Set_datas(&g - &this->sight_data[0], EnumGunParts::PARTS_SIGHT); }
			for (auto& g : this->magazine_data) { g.Set_datas(&g - &this->magazine_data[0], EnumGunParts::PARTS_MAGAZINE); }
			for (auto& g : this->gun_data) { g.Set_datas(&g - &this->gun_data[0], EnumGunParts::PARTS_BASE); }
			//MEDデータ2
			for (auto& g : this->meds_data) { g.Set_datas(&g - &this->meds_data[0]); }
			//弾痕
			this->hit_buf = 0;
			this->hit_obj.resize(64);
			for (auto& h : this->hit_obj) { h.Set(this->hit_pic); }
		}
		void Set_Charaa(const size_t &spawn_total) {
			//キャラ設定
			this->chara.resize(spawn_total);
			for (auto& c : this->chara) {
				c.Set(MAPPTs, DrawPts, gun_data, 0, body_obj, body_obj_lag, body_col);
			}
		}
		void Set() {
			for (auto& c : this->chara) {
				//カスタムattach
				if (&c != &Get_Mine()) {
					//magazine
					c.Attach_parts(&magazine_data[0], EnumGunParts::PARTS_MAGAZINE, nullptr, 0);
					//grip
					c.Attach_parts(&grip_data[0], EnumGunParts::PARTS_GRIP, &c.base, EnumAttachPoint::GRIP_BASE);
					//uperhandguard
					c.Attach_parts(&uperhandguard_data[0], EnumGunParts::PARTS_UPER_HGUARD, &c.base, EnumAttachPoint::UPER_HANDGUARD);
					{
						c.Attach_parts(&sight_data[0], EnumGunParts::PARTS_SIGHT, c.get_parts(EnumGunParts::PARTS_UPER_HGUARD), EnumAttachPoint::UPER_RAIL);
					}
					//underhandguard
					c.Attach_parts(&underhandguard_data[0], EnumGunParts::PARTS_UNDER_HGUARD, &c.base, EnumAttachPoint::UNDER_HANDGUARD);
					{
						c.Attach_parts(&foregrip_data[0], EnumGunParts::PARTS_FOREGRIP, c.get_parts(EnumGunParts::PARTS_UNDER_HGUARD), EnumAttachPoint::UNDER_RAIL);
						c.Attach_parts(&light_data[0], EnumGunParts::PARTS_LIGHT, c.get_parts(EnumGunParts::PARTS_UNDER_HGUARD), EnumAttachPoint::LEFTSIDE_RAIL);
						c.Attach_parts(&laser_data[0], EnumGunParts::PARTS_LASER, c.get_parts(EnumGunParts::PARTS_UNDER_HGUARD), EnumAttachPoint::RIGHTSIDE_RAIL);
					}
					//sidemount
					c.Attach_parts(&mount_data[0], EnumGunParts::PARTS_MOUNT, &c.base, EnumAttachPoint::SIDEMOUNT_BASE);
					{
						c.Attach_parts(&mount_data[1], EnumGunParts::PARTS_MOUNT, &c.mount_[0], EnumAttachPoint::SIDEMOUNT);
						{
							c.Attach_parts(&sight_data[1], EnumGunParts::PARTS_SIGHT, &c.mount_[1], EnumAttachPoint::UPER_RAIL);
						}
					}
					//optional
					c.Attach_parts(&mazzule_data[0], EnumGunParts::PARTS_MAZZULE, &c.base, EnumAttachPoint::MAZZULE_BASE);
					c.Attach_parts(&dustcover_data[0], EnumGunParts::PARTS_DUSTCOVER, &c.base, EnumAttachPoint::DUSTCOVER_BASE);
					//c.Attach_parts(&stock_data[0], EnumGunParts::PARTS_STOCK, &c.base, EnumAttachPoint::STOCK_BASE);
					//
				}
				//初回位置設定スポーン
				{
					auto&wp = (*MAPPTs)->get_spawn_point()[&c - &this->chara[0]];
					c.spawn(wp, MATRIX_ref::RotY(atan2f(wp.x(), wp.z())));
				}
			}
			//ライティング
			Shadow_maxpos = (*MAPPTs)->map_col_get().mesh_maxpos(0);
			Shadow_minpos = (*MAPPTs)->map_col_get().mesh_minpos(0);
			Light_vec = VGet(0.5f, -0.5f, 0.5f);
			Light_color = GetColorF(0.42f, 0.41f, 0.40f, 0.0f);
			//
			(*MAPPTs)->Set();																				//環境
			std::for_each(this->chara.begin(), this->chara.end(), [&](PLAYERclass::Chara& c) { c.Start(); });	//プレイヤー操作変数群
			SetMousePoint(deskx / 2, desky / 2);														//
			fov_base = deg2rad(use_VR ? 120 : (*OPTPTs)->Fov);
			camera_main.set_cam_info(fov_base, 0.1f, 200.f);											//1P
			TPSparts->Set((*OPTPTs)->Fov);																	//TPS
			RULEparts->Set();																			//ルール
		}
		bool UpDate() {
			//共通演算
			for (auto& c : this->chara) {
				c.UpDate(
					this->hit_obj, this->hit_buf, 
					RULEparts->get_Playing(), this->camera_main.fov / this->fov_base, this->meds_data,
					this->chara, this->Get_Mine(), use_VR
				);
			}
			//弾痕の更新
			for (auto&s : hit_obj) { s.UpDate(); }
			//campos,camvec,camupの指定
			Get_Mine().Set_cam(camera_main, this->chara, fov_base, use_VR);
			//ルール保存
			UIparts->Update();
			RULEparts->UpDate();
			//ミニマップ
			minimapparts->Set(this->chara, (TPSparts->key_TPS.on()) ? this->chara[TPSparts->sel_cam] : Get_Mine());
			//TPS描画
			TPSparts->key_TPS.get_in(Get_Mine().get_key_().TPS);
			TPSparts->Set_info(this->chara);
			if (TPSparts->key_TPS.on()) {
				//影用意
				(*DrawPts)->Ready_Shadow(TPSparts->camera_TPS.campos, [&] {Shadow_Draw(); }, VGet(2.f, 2.5f, 2.f), VGet(5.f, 2.5f, 5.f));
				//書き込み
				Hostpassparts_TPS->BUF_draw([&](void) { BG_Draw(); }, [&] {(*DrawPts)->Draw_by_Shadow([&] {Main_Draw(); }); }, TPSparts->camera_TPS);	//被写体深度描画
				Hostpassparts_TPS->Set_MAIN_draw();																												//最終描画
			}
			//表示の如何
			{
				cam_info* camp = TPSparts->key_TPS.on() ? &TPSparts->camera_TPS : &camera_main;
				GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), camp->campos, camp->camvec, camp->camup, camp->fov, camp->near_, camp->far_);
				{
					for (auto& c : this->chara) {
						c.Check_CameraViewClip(/**MAPPTs*/);
					}
				}
			}
			//終了判定
			if (RULEparts->get_timer() <= -5.f) {
				return false;
			}
			return true;
		}
		void Dispose() {
			for (auto& c : this->chara) {
				c.Dispose();
			}
			this->chara.clear();
		}
		void UI_Draw(void) {
			UIparts->UI_Draw(Get_Mine(), use_VR);
		}
		void BG_Draw() {
			//map
			(*MAPPTs)->sky_Draw();
		}
		void Shadow_Draw_Far() {
			//map
			(*MAPPTs)->Shadow_Draw();
		}
		void Shadow_Draw() {
			//map
			(*MAPPTs)->item_Draw();
			//キャラ
			for (auto& c : this->chara) { c.Draw_chara(); }
		}
		void Main_Draw() {
			//map
			(*MAPPTs)->main_Draw();
			//命中痕
			for (auto&s : hit_obj) { s.Draw(); }
			//サイト
			for (auto& s : Get_Mine().sight_) { s.Draw_reticle(); }
			//キャラ
			std::for_each(this->chara.begin(), this->chara.end(), [&](PLAYERclass::Chara& c) { c.Draw_chara(); });
			//レーザー
			std::for_each(this->chara.begin(), this->chara.end(), [&](PLAYERclass::Chara& c) { c.Draw_laser(this->chara, light); });
			//銃弾
			SetFogEnable(FALSE);
			SetUseLighting(FALSE);
			std::for_each(this->chara.begin(), this->chara.end(), [&](PLAYERclass::Chara& c) { c.Draw_ammo(); });
			SetUseLighting(TRUE);
			SetFogEnable(TRUE);
			//
		}
		void Item_Draw() {
			for (auto& c : this->chara) {
				c.Set_Draw_bullet();
			}
			UIparts->item_Draw(this->chara, Get_Mine(), camera_main.campos, use_VR);
		}
		void LAST_Draw(void) {
			//TPS視点
			if (TPSparts->key_TPS.on()) {
				Hostpassparts_TPS->MAIN_draw();
			}
			//minimap
			minimapparts->Draw(64, 64);
		}
	};
};