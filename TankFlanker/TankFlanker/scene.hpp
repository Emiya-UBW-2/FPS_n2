#pragma once
class Sceneclass : Mainclass {
private:
	//TPS操作
	class TPS_parts {
	public:
		switchs key_TPS;
		float xr_cam = 0.f;
		float yr_cam = 0.f;
		int sel_cam = 0;
		//カメラ
		cam_info camera_TPS;
		void Set(float& fov_pc) {
			this->key_TPS.ready(false);
			this->camera_TPS.campos = VGet(0, 1.8f, -10);
			this->camera_TPS.set_cam_info(deg2rad(fov_pc), 0.1f, 200.f);
		}
		void Set_info(std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>& MAPPTs, std::vector<Chara>&chara) {
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
						this->camera_TPS.campos.x(std::clamp(this->camera_TPS.campos.x(), MAPPTs->map_col_get().mesh_minpos(0).x(), MAPPTs->map_col_get().mesh_maxpos(0).x()));
						this->camera_TPS.campos.z(std::clamp(this->camera_TPS.campos.z(), MAPPTs->map_col_get().mesh_minpos(0).z(), MAPPTs->map_col_get().mesh_maxpos(0).z()));
					}
					//rad
					{
						VECTOR_ref vec_2 = (chara[this->sel_cam].get_head_frame() - this->camera_TPS.campos).Norm();
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
public:
	class SELECT;
	class MAINLOOP;
	//
	class SELECT {
		//
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
		Chara::g_parts* port_ptr = nullptr;
		std::vector<Chara::g_parts*> sight_ptr;
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
		std::vector<int> sight_i;
		bool Start_b = true;
		float rate = 1.0f;
		Chara* mine_ptr = nullptr;
		//
		VECTOR_ref Shadow_minpos;
		VECTOR_ref Shadow_maxpos;
		VECTOR_ref Light_vec;
		COLOR_F Light_color;
		//
		float change_per = 1.f;
		//
		class save_c {
		public:
			size_t cang_ = 0;					//パーツ選択
			size_t type_ = 0;					//パーツの種類
			size_t pt_cat_ = SIZE_MAX;			//ベースパーツの場所
			size_t pt_type_ = 0;				//ベースパーツの種類
			size_t pt_sel_ = 0;					//ベースパーツの番号(マウントなど)
			/*
			parts_p = &MAINLOOPscene->get_uperhandguard_data()[0];
			mine_ptr->add_parts(parts_p, EnumGunParts::PARTS_UPER_HGUARD, &mine_ptr->base, EnumAttachPoint::UPER_HANDGUARD);
			*/
		};
		std::vector<save_c> save_parts;

		//
		std::unique_ptr<UIclass::UI_SELECT, std::default_delete<UIclass::UI_SELECT>> UIparts;
	public:
	private:
		//必須品
		void Set_chang_haveto(std::vector<GUNPARTs>&data, const size_t& pts_cat, const size_t& pot_cat, int& chang_t) {
			mine_ptr->delete_parts(pts_cat);

			chang_max = (int)data.size();
			chang_t = std::clamp(chang_t, 0, chang_max - 1);
			parts_p = &data[chang_t];

			mine_ptr->add_parts(parts_p, pts_cat, port_ptr, pot_cat);

			viewparts_buf = port_ptr->obj.frame(port_ptr->rail_f[pot_cat][0].first);
		}
		//非必須品
		void Set_chang_needs(std::vector<GUNPARTs>&data, const size_t& pts_cat, const size_t& pot_cat, int& chang_t, const size_t& sel) {
			chang_max = (int)data.size() + 1;
			chang_t = std::clamp(chang_t, 0, chang_max - 1);
			if (chang_t == 0) { parts_p = nullptr; }
			else { parts_p = &data[std::max(chang_t - 1, 0)]; }
			viewparts_buf = port_ptr->obj.frame(port_ptr->rail_f[pot_cat][0].first);

			switch (sel) {
			case 0:
			{
				mine_ptr->delete_parts(pts_cat);
				break;
			}
			default:
			{
				if (sight_i.size() < sel) {
					sight_i.resize(sel);
				}
				if (sight_i[sel - 1] > 0) {
					mine_ptr->delete_parts(pts_cat, sight_i[sel - 1] - 1);
					if (sight_i[sel - 1] != 3) {
						for (auto&s : sight_i) {
							if (s > 1) {
								s--;
							}
						}
					}
				}
				break;
			}
			}
			if (chang_t != 0) {
				mine_ptr->add_parts(parts_p, pts_cat, port_ptr, pot_cat);
			}
			if (sel > 0) {
				int tgt = int((chang_t != 0) ? mine_ptr->sight_.size() : 0);
				sight_i[sel - 1] = tgt;
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
			if (chang_t != 0) {
				mine_ptr->add_parts(parts_p, pts_cat, port_ptr, pot_cat);
			}
			viewparts_buf = port_ptr->obj.frame(port_ptr->rail_f[pot_cat][0].first);
		}
		//
		bool Set_sight_at(Chara::g_parts* ports_ptr, std::vector<GUNPARTs>&data) {
			bool trm = false;
			if (ports_ptr->get_attaching()) {
				for (auto& can : ports_ptr->thisparts->can_attach) {
					for (auto& tgt : data) {
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
		auto& get_camera(void) { return camera_main; }
		//
		SELECT(std::unique_ptr<DXDraw, std::default_delete<DXDraw>>& DrawPts) {
			use_VR = DrawPts->use_vr;
			this->assemble = SoundHandle::Load("data/audio/assemble.wav");
			this->decision = SoundHandle::Load("data/audio/shot_2.wav");//
			this->cancel = SoundHandle::Load("data/audio/cancel.wav");
			this->cursor = SoundHandle::Load("data/audio/cursor.wav");
			UIparts = std::make_unique<UIclass::UI_SELECT>();
		}
		void Set(std::unique_ptr<OPTION, std::default_delete<OPTION>>& OPTPTs, std::unique_ptr<MAINLOOP, std::default_delete<MAINLOOP>>& MAINLOOPscene) {
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
				sight_i.clear();
				Start_b = true;
				rate = 0.1f;
			}
			{
				mine_ptr = &MAINLOOPscene->get_mine();
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
			fov_base = deg2rad(use_VR ? 180 : OPTPTs->Fov);
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
				file.open("data/save/1.dat", std::ios::binary | std::ios::in);
				int cnt = 0;
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
				for (auto&tmp_save : save_parts) {
					GUNPARTs* temp_p = nullptr;
					Chara::g_parts* temp_ptr = nullptr;
					{
						if (tmp_save.cang_ != SIZE_MAX) {
							switch (tmp_save.type_) {
							case EnumGunParts::PARTS_MAGAZINE:
								temp_p = &MAINLOOPscene->get_magazine_data()[tmp_save.cang_];
								break;
							case EnumGunParts::PARTS_GRIP:
								temp_p = &MAINLOOPscene->get_grip_data()[tmp_save.cang_];
								break;
							case EnumGunParts::PARTS_UPER_HGUARD:
								temp_p = &MAINLOOPscene->get_uperhandguard_data()[tmp_save.cang_];
								break;
							case EnumGunParts::PARTS_UNDER_HGUARD:
								temp_p = &MAINLOOPscene->get_underhandguard_data()[tmp_save.cang_];
								break;
							case EnumGunParts::PARTS_MAZZULE:
								temp_p = &MAINLOOPscene->get_mazzule_data()[tmp_save.cang_];
								break;
							case EnumGunParts::PARTS_DUSTCOVER:
								temp_p = &MAINLOOPscene->get_dustcover_data()[tmp_save.cang_];
								break;
							case EnumGunParts::PARTS_STOCK:
								temp_p = &MAINLOOPscene->get_stock_data()[tmp_save.cang_];
								break;
							case EnumGunParts::PARTS_LIGHT:
								temp_p = &MAINLOOPscene->get_light_data()[tmp_save.cang_];
								break;
							case EnumGunParts::PARTS_LASER:
								temp_p = &MAINLOOPscene->get_laser_data()[tmp_save.cang_];
								break;
							case EnumGunParts::PARTS_MOUNT:
								temp_p = &MAINLOOPscene->get_mount_data()[tmp_save.cang_];
								break;
							case EnumGunParts::PARTS_SIGHT:
								temp_p = &MAINLOOPscene->get_sight_data()[tmp_save.cang_];
								break;
							case EnumGunParts::PARTS_FOREGRIP:
								temp_p = &MAINLOOPscene->get_foregrip_data()[tmp_save.cang_];
								break;
							default:
								break;
							}
						}
					}
					{
						switch (tmp_save.pt_type_) {
						case EnumGunParts::PARTS_BASE:
							temp_ptr = &mine_ptr->base;
							break;
						case EnumGunParts::PARTS_UPER_HGUARD:
							temp_ptr = &mine_ptr->uperhandguard;
							break;
						case EnumGunParts::PARTS_UNDER_HGUARD:
							temp_ptr = &mine_ptr->underhandguard;
							break;
						case EnumGunParts::PARTS_DUSTCOVER:
							temp_ptr = &mine_ptr->dustcover;
							break;
						case EnumGunParts::PARTS_MOUNT:
							if (tmp_save.pt_sel_ != SIZE_MAX) {
								temp_ptr = &mine_ptr->mount_[tmp_save.pt_sel_];
							}
							break;
						default:
							break;
						}
					}
					mine_ptr->add_parts(temp_p, tmp_save.type_, temp_ptr, tmp_save.pt_cat_);
				}
				//save_parts.clear();
			}
		}
		bool UpDate(std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>& MAPPTs, std::unique_ptr<DXDraw, std::default_delete<DXDraw>>& DrawPts, std::unique_ptr<MAINLOOP, std::default_delete<MAINLOOP>>& MAINLOOPscene, key_bind& k_) {
			{
				bool changef = false;
				//演算
				{
					{
						mine_ptr->set_gun(VGet(0, 0, 0), MGetIdent());
						mine_ptr->Set_gun();
						mine_ptr->set_mag();
						mine_ptr->Set_mag();

						up.get_in(mine_ptr->get_wkey());
						down.get_in(mine_ptr->get_skey());
						left.get_in(mine_ptr->get_akey());
						right.get_in(mine_ptr->get_dkey());
						shot.get_in(mine_ptr->get_shoot());
						Start.get_in(k_.key_use_ID[14].get_key(0));
						Rot.get_in(k_.mouse_use_ID[1].get_key(3));
						{
							k_.key_use_ID[0].isalways = true;
							k_.key_use_ID[1].isalways = true;
							k_.key_use_ID[2].isalways = true;
							k_.key_use_ID[3].isalways = true;

							k_.key_use_ID[10].isalways = true;
							k_.key_use_ID[14].isalways = true;

							k_.mouse_use_ID[0].isalways = true;
							k_.mouse_use_ID[1].isalways = true;
						}
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
					{
						sight_ptr.resize(5);
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
								Set_chang_haveto(MAINLOOPscene->get_magazine_data(), parts_cat, port_cat, chang);
								{
									if (save_parts.size() < sel_max + 1) {
										save_parts.resize(sel_max + 1);
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
								Set_chang_haveto(MAINLOOPscene->get_grip_data(), parts_cat, port_cat, chang);
								{
									if (save_parts.size() < sel_max + 1) {
										save_parts.resize(sel_max + 1);
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
								Set_chang_haveto(MAINLOOPscene->get_uperhandguard_data(), parts_cat, port_cat, chang);
								//
								if (Set_sight_at(&mine_ptr->uperhandguard, MAINLOOPscene->get_sight_data())) {
									sight_ptr[0] = &mine_ptr->uperhandguard;
								}
								//
								{
									if (save_parts.size() < sel_max + 1) {
										save_parts.resize(sel_max + 1);
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
								Set_chang_haveto(MAINLOOPscene->get_underhandguard_data(), parts_cat, port_cat, chang);
								{
									if (save_parts.size() < sel_max + 1) {
										save_parts.resize(sel_max + 1);
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
							port_ptr = &mine_ptr->underhandguard;
							port_cat = EnumAttachPoint::UNDER_RAIL;
							if (port_ptr->rail_f[port_cat][0].first > 0) {
								if (sel_p == sel_max) {
									parts_cat = EnumGunParts::PARTS_FOREGRIP;
									Set_chang_needs(MAINLOOPscene->get_foregrip_data(), parts_cat, port_cat, chang, 0);
									{
										if (save_parts.size() < sel_max + 1) {
											save_parts.resize(sel_max + 1);
										}
										save_parts[sel_max].cang_ = chang - 1;
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
							port_ptr = &mine_ptr->underhandguard;
							port_cat = EnumAttachPoint::LEFTSIDE_RAIL;
							if (port_ptr->rail_f[port_cat][0].first > 0) {
								if (sel_p == sel_max) {
									if (chang == 0) {
										parts_cat = EnumGunParts::PARTS_LIGHT;
									}
									else {
										parts_cat = (chang < MAINLOOPscene->get_light_data().size() + 1) ? EnumGunParts::PARTS_LIGHT : EnumGunParts::PARTS_LASER;
									}
									mine_ptr->delete_parts(EnumGunParts::PARTS_LIGHT);
									mine_ptr->delete_parts(EnumGunParts::PARTS_LASER);
									Set_chang_need3(MAINLOOPscene->get_light_data(), MAINLOOPscene->get_laser_data(), parts_cat, port_cat, chang);
									{
										auto chang_t = chang;
										if (chang != 0) {
											if (chang < MAINLOOPscene->get_light_data().size() + 1) { chang_t = std::max(chang - 1, 0); }
											else { chang_t = std::max(chang - 1, 0) - MAINLOOPscene->get_light_data().size(); }
										}

										if (save_parts.size() < sel_max + 1) {
											save_parts.resize(sel_max + 1);
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
									else { parts_p = &MAINLOOPscene->get_mount_data()[std::max(chang - 1, 0)]; }
									mine_ptr->delete_parts(parts_cat, 0);
									if (chang != 0) {
										mine_ptr->add_parts(parts_p, parts_cat, port_ptr, port_cat);
									}
									viewparts_buf = port_ptr->obj.frame(port_ptr->rail_f[port_cat][0].first);
								}
								{
									if (save_parts.size() < sel_max + 1) {
										save_parts.resize(sel_max + 1);
									}
									save_parts[sel_max].cang_ = chang - 1;
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
											else { parts_p = &MAINLOOPscene->get_mount_data()[chang]; }
											if (mine_ptr->mount_.size() == 2) {
												mine_ptr->delete_parts(parts_cat, 1);
											}
											if (chang != 0) {
												mine_ptr->add_parts(parts_p, parts_cat, port_ptr, port_cat);
											}
											viewparts_buf = port_ptr->obj.frame(port_ptr->rail_f[port_cat][0].first);
										}
										//
										if (mine_ptr->mount_.size() > 1) {
											if (Set_sight_at(&mine_ptr->mount_[1], MAINLOOPscene->get_sight_data())) {
												sight_ptr[1] = &mine_ptr->mount_[1];
											}
										}
										//
										{
											if (save_parts.size() < sel_max + 1) {
												save_parts.resize(sel_max + 1);
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
								Set_chang_needs(MAINLOOPscene->get_mazzule_data(), parts_cat, port_cat, chang, 0);
								{
									if (save_parts.size() < sel_max + 1) {
										save_parts.resize(sel_max + 1);
									}
									save_parts[sel_max].cang_ = chang - 1;
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
								Set_chang_needs(MAINLOOPscene->get_dustcover_data(), parts_cat, port_cat, chang, 0);
								//
								if (Set_sight_at(&mine_ptr->dustcover, MAINLOOPscene->get_sight_data())) {
									sight_ptr[2] = &mine_ptr->dustcover;
								}
								//
								{
									if (save_parts.size() < sel_max + 1) {
										save_parts.resize(sel_max + 1);
									}
									save_parts[sel_max].cang_ = chang - 1;
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
								Set_chang_needs(MAINLOOPscene->get_stock_data(), parts_cat, port_cat, chang, 0);
								{
									if (save_parts.size() < sel_max + 1) {
										save_parts.resize(sel_max + 1);
									}
									save_parts[sel_max].cang_ = chang - 1;
									save_parts[sel_max].type_ = parts_cat;
									save_parts[sel_max].pt_cat_ = port_cat;
									save_parts[sel_max].pt_type_ = EnumGunParts::PARTS_BASE;
									save_parts[sel_max].pt_sel_ = 0;
								}
							}
							sel_max++;
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
											Set_chang_needs(MAINLOOPscene->get_sight_data(), parts_cat, port_cat, chang, 1 + sel_max - ssp);
											{
												if (save_parts.size() < sel_max + 1) {
													save_parts.resize(sel_max + 1);
												}
												save_parts[sel_max].cang_ = chang - 1;
												save_parts[sel_max].type_ = parts_cat;
												save_parts[sel_max].pt_cat_ = port_cat;

												save_parts[sel_max].pt_type_ = s->thisparts->type;
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
								mine_ptr->calc_cart(rate);
							}
							//エフェクト
							mine_ptr->calc_shot_effect();
						}
					}
					//
					mine_ptr->Set_select();
					//
					mine_ptr->Set_shot(rate);
					//
					mine_ptr->base.obj.work_anime();

					//薬莢の処理
					mine_ptr->update_cart(MAPPTs);
					mine_ptr->update_effect(DrawPts);
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
							if (mine_ptr->mazzule.get_mazzuletype() > 0) {
								tmp = VECTOR_ref(mine_ptr->mazzule.mazzule_pos()) - tmp;
							}
							else {
								tmp = mine_ptr->base.obj.frame(mine_ptr->base.thisparts->frame_s.mazzule_f.first) - tmp;
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
			//終了判定
			if (k_.key_use_ID[11].get_key(0)) {
				return false;
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
				file.open("data/save/1.dat", std::ios::binary | std::ios::out);
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
		void UI_Draw(std::unique_ptr<MAINLOOP, std::default_delete<MAINLOOP>>& MAINLOOPscene) {
			UIparts->UI_Draw(MAINLOOPscene, parts_cat, Rot, mine_ptr, parts_p, change_per, use_VR);
			easing_set(&change_per, 0.f, 0.5f);
		}
		void BG_Draw(void) {
			DrawBox(0, 0, 1920, 1080, GetColor(192, 192, 192), TRUE);
		}
		void Shadow_Draw_Far() {

		}
		void Shadow_Draw(void) {
			mine_ptr->Draw_gun();
			mine_ptr->magazine.Draw(2);
		}
		void Main_Draw(void) {
			mine_ptr->Draw_gun();
			mine_ptr->magazine.Draw(2);
		}
	};
	//
	class MAINLOOP {
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
		std::vector<Chara> chara;					//キャラ
		std::vector<Items> item;					//アイテム
		std::vector<Hit> hit_obj;					//弾痕
		size_t hit_buf = 0;							//
		//
		std::unique_ptr<HostPassEffect, std::default_delete<HostPassEffect>> Hostpassparts_TPS;
		std::unique_ptr<TPS_parts, std::default_delete<TPS_parts>> TPSparts;
		std::unique_ptr<UIclass::UI_MAINLOOP, std::default_delete<UIclass::UI_MAINLOOP>> UIparts;
		std::unique_ptr<RULE_parts, std::default_delete<RULE_parts>> RULEparts;
		std::unique_ptr<MAPclass::MiniMap, std::default_delete<MAPclass::MiniMap>> minimapparts;
		//
		VECTOR_ref Shadow_minpos;
		VECTOR_ref Shadow_maxpos;
		VECTOR_ref Light_vec;
		COLOR_F Light_color;
	public:
		//
		auto& get_Shadow_minpos() { return Shadow_minpos; }
		auto& get_Shadow_maxpos() { return Shadow_maxpos; }
		auto& get_Light_vec() { return Light_vec; }
		auto& get_Light_color() { return Light_color; }
		std::vector<Chara>& get_chara(void) { return this->chara; }
		std::vector<Items>& get_items(void) { return this->item; }
		Chara& get_mine(void) { return this->chara[0]; }
		std::vector<GUNPARTs>& get_mazzule_data(void) { return this->mazzule_data; }
		std::vector<GUNPARTs>& get_grip_data(void) { return this->grip_data; }
		std::vector<GUNPARTs>& get_uperhandguard_data(void) { return this->uperhandguard_data; }
		std::vector<GUNPARTs>& get_underhandguard_data(void) { return this->underhandguard_data; }
		std::vector<GUNPARTs>& get_stock_data(void) { return this->stock_data; }
		std::vector<GUNPARTs>& get_dustcover_data(void) { return this->dustcover_data; }
		std::vector<GUNPARTs>& get_sight_data(void) { return this->sight_data; }
		std::vector<GUNPARTs>& get_foregrip_data(void) { return this->foregrip_data; }
		std::vector<GUNPARTs>& get_light_data(void) { return this->light_data; }
		std::vector<GUNPARTs>& get_laser_data(void) { return this->laser_data; }
		std::vector<GUNPARTs>& get_mount_data(void) { return this->mount_data; }
		std::vector<GUNPARTs>& get_gun_data(void) { return this->gun_data; }
		std::vector<GUNPARTs>& get_magazine_data(void) { return this->magazine_data; }
		std::vector<Meds>& get_meds_data(void) { return this->meds_data; }
		auto& get_camera(void) { return this->camera_main; }
		//
		MAINLOOP(std::unique_ptr<DXDraw, std::default_delete<DXDraw>>& DrawPts, std::unique_ptr<OPTION, std::default_delete<OPTION>>& OPTPTs) {
			//
			use_VR = DrawPts->use_vr;
			//
			UIparts = std::make_unique<UIclass::UI_MAINLOOP>();
			TPSparts = std::make_unique<TPS_parts>();
			RULEparts = std::make_unique<RULE_parts>();
			Hostpassparts_TPS = std::make_unique<HostPassEffect>(OPTPTs->DoF, OPTPTs->Bloom, OPTPTs->SSAO, deskx, desky);	//ホストパスエフェクト(フルスクリーン向け、TPS用)
			minimapparts = std::make_unique<MAPclass::MiniMap>();															//ミニマップ
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
				this->uperhandguard_data.resize(4);
				this->uperhandguard_data[0].mod.Ready("data/parts/uper_handguard/", "00_AK_6P1_Uper_Handguard");
				this->uperhandguard_data[1].mod.Ready("data/parts/uper_handguard/", "01_AK_Original_Polymer_Uper_Handguard");
				this->uperhandguard_data[2].mod.Ready("data/parts/uper_handguard/", "02_AK_Steel_Uper_Handguard");
				this->uperhandguard_data[3].mod.Ready("data/parts/uper_handguard/", "03_AK_Rail_Band");
				this->underhandguard_data.resize(3);
				this->underhandguard_data[0].mod.Ready("data/parts/under_handguard/", "00_AK_6P1_Under_Handguard");
				this->underhandguard_data[1].mod.Ready("data/parts/under_handguard/", "01_AK_Original_Polymer_Under_Handguard");
				this->underhandguard_data[2].mod.Ready("data/parts/under_handguard/", "02_AK_Zenit_B10M_Under_Handguard");
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
		void first_after() {
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
				c.Set(gun_data, 0, body_obj, body_obj_lag, body_col);
			}
		}
		void Set(std::unique_ptr<OPTION, std::default_delete<OPTION>>& OPTPTs, std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>& MAPPTs) {
			for (auto& c : this->chara) {
				//カスタムattach
				if (&c != &get_mine()) {
					//magazine
					c.add_parts(&magazine_data[1], EnumGunParts::PARTS_MAGAZINE, nullptr, 0);
					//grip
					c.add_parts(&grip_data[0], EnumGunParts::PARTS_GRIP, &c.base, EnumAttachPoint::GRIP_BASE);
					//uperhandguard
					c.add_parts(&uperhandguard_data[0], EnumGunParts::PARTS_UPER_HGUARD, &c.base, EnumAttachPoint::UPER_HANDGUARD);
					{
						c.add_parts(&sight_data[0], EnumGunParts::PARTS_SIGHT, &c.uperhandguard, EnumAttachPoint::UPER_RAIL);
					}
					//underhandguard
					c.add_parts(&underhandguard_data[0], EnumGunParts::PARTS_UNDER_HGUARD, &c.base, EnumAttachPoint::UNDER_HANDGUARD);
					{
						c.add_parts(&foregrip_data[0], EnumGunParts::PARTS_FOREGRIP, &c.underhandguard, EnumAttachPoint::UNDER_RAIL);
						c.add_parts(&light_data[0], EnumGunParts::PARTS_LIGHT, &c.underhandguard, EnumAttachPoint::LEFTSIDE_RAIL);
						c.add_parts(&laser_data[0], EnumGunParts::PARTS_LASER, &c.underhandguard, EnumAttachPoint::RIGHTSIDE_RAIL);
					}
					//sidemount
					c.add_parts(&mount_data[0], EnumGunParts::PARTS_MOUNT, &c.base, EnumAttachPoint::SIDEMOUNT_BASE);
					{
						c.add_parts(&mount_data[1], EnumGunParts::PARTS_MOUNT, &c.mount_[0], EnumAttachPoint::SIDEMOUNT);
						{
							c.add_parts(&sight_data[1], EnumGunParts::PARTS_SIGHT, &c.mount_[1], EnumAttachPoint::UPER_RAIL);
						}
					}
					//optional
					c.add_parts(&mazzule_data[0], EnumGunParts::PARTS_MAZZULE, &c.base, EnumAttachPoint::MAZZULE_BASE);
					c.add_parts(&dustcover_data[0], EnumGunParts::PARTS_DUSTCOVER, &c.base, EnumAttachPoint::DUSTCOVER_BASE);
					//c.add_parts(&stock_data[0], EnumGunParts::PARTS_STOCK, &c.base, EnumAttachPoint::STOCK_BASE);
					//
				}
				//spawn
				{
					auto&wp = MAPPTs->get_spawn_point()[&c - &this->chara[0]];
					c.spawn(wp, MATRIX_ref::RotY(atan2f(wp.x(), wp.z())));
				}
			}
			//ライティング
			Shadow_maxpos = MAPPTs->map_col_get().mesh_maxpos(0);
			Shadow_minpos = MAPPTs->map_col_get().mesh_minpos(0);
			Light_vec = VGet(0.5f, -0.5f, 0.5f);
			Light_color = GetColorF(0.42f, 0.41f, 0.40f, 0.0f);
			//
			MAPPTs->Start_map();															//環境
			std::for_each(this->chara.begin(), this->chara.end(), [&](Chara& c) { c.Start(MAPPTs); });//プレイヤー操作変数群
			SetMousePoint(deskx / 2, desky / 2);											//
			fov_base = deg2rad(use_VR ? 120 : OPTPTs->Fov);
			camera_main.set_cam_info(fov_base, 0.1f, 200.f);//1P
			TPSparts->Set(OPTPTs->Fov);															//TPS
			RULEparts->Set();																//ルール
		}
		bool UpDate(std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>& MAPPTs, std::unique_ptr<DXDraw, std::default_delete<DXDraw>>& DrawPts, key_bind& k_) {
			{
				//演算
				{
					//
					if (use_VR) {
					}
					else {
						k_.key_use_ID[0].isalways = true;
						k_.key_use_ID[1].isalways = true;
						k_.key_use_ID[2].isalways = true;
						k_.key_use_ID[3].isalways = true;
						k_.key_use_ID[4].isalways = true;
						k_.key_use_ID[5].isalways = true;
						k_.key_use_ID[6].isalways = true;
						k_.key_use_ID[7].isalways = true;
						k_.key_use_ID[8].isalways = true;
						k_.key_use_ID[9].isalways = true;
						k_.key_use_ID[10].isalways = true;
						k_.key_use_ID[11].isalways = true;
						k_.key_use_ID[12].isalways = true;
						k_.key_use_ID[13].isalways = true;
						k_.key_use_ID[14].isalways = true;
						k_.key_use_ID[15].isalways = true;
						k_.key_use_ID[16].isalways = true;

						k_.mouse_use_ID[0].isalways = true;
						k_.mouse_use_ID[1].isalways = true;
						k_.mouse_use_ID[2].isalways = true;
					}
					//アイテム演算
					for (auto& g : this->item) {
						g.UpDate(this->item, MAPPTs);
					}
					//共通
					for (auto& c : this->chara) {
						c.UpDate(MAPPTs, DrawPts, this->hit_obj, this->hit_buf, RULEparts->get_Playing(), this->camera_main.fov / this->fov_base, this->meds_data, this->item, this->chara, this->get_mine(), use_VR);
					}
					//空マガジンを削除する
					while (true) {
						bool demagazine_flag = false;
						for (auto& i : this->item) {
							if (i.Delete_mag()) {
								demagazine_flag = true;
								this->item.erase(this->item.begin() + (&i - &this->item[0]));
								break;
							}
						}
						if (!demagazine_flag) {
							break;
						}
					}
					//弾痕の更新
					for (auto&s : hit_obj) { s.UpDate(); }
				}
				{
					//campos,camvec,camupの指定
					get_mine().Set_cam(MAPPTs, camera_main, this->chara, fov_base, use_VR);
				}
				//ルール保存
				UIparts->Update(RULEparts);
				RULEparts->UpDate();
				//TPS
				TPSparts->key_TPS.get_in(k_.key_use_ID[15].get_key(0));
				//ミニマップ
				minimapparts->Set(this->chara, (TPSparts->key_TPS.on()) ? this->chara[TPSparts->sel_cam] : get_mine(), MAPPTs);
				//TPS描画
				TPSparts->Set_info(MAPPTs, this->chara);
				if (TPSparts->key_TPS.on()) {
					//影用意
					DrawPts->Ready_Shadow(TPSparts->camera_TPS.campos, [&] {Shadow_Draw(); }, VGet(2.f, 2.5f, 2.f), VGet(5.f, 2.5f, 5.f));
					//書き込み
					Hostpassparts_TPS->BUF_draw([&](void) { BG_Draw(MAPPTs); }, [&] {DrawPts->Draw_by_Shadow([&] {Main_Draw(MAPPTs); }); }, TPSparts->camera_TPS);	//被写体深度描画
					Hostpassparts_TPS->MAIN_draw();																													//最終描画
				}
				//
				{
					cam_info* camp = TPSparts->key_TPS.on() ? &camera_main : &TPSparts->camera_TPS;
					GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), camp->campos, camp->camvec, camp->camup, camp->fov, camp->near_, camp->far_);
					{
						for (auto& c : this->chara) {
							c.Check_CameraViewClip(/*MAPPTs*/);
						}
					}
				}
			}
			//終了判定
			if (k_.key_use_ID[11].get_key(0)) {
				return false;
			}
			if (k_.key_use_ID[10].get_key(0)) {
				return false;
			}
			return true;
			//
		}
		void Dispose() {
			for (auto& c : this->chara) {
				c.Delete_chara();
			}
			this->chara.clear();
			for (auto& i : this->item) {
				i.Delete_item();
			}
			this->item.clear();
		}
		void UI_Draw(void) {
			UIparts->UI_Draw(get_mine(), use_VR);
		}
		void BG_Draw(std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>& MAPPTs) {
			MAPPTs->sky_Draw();
		}
		void Shadow_Draw_Far(std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>& MAPPTs) {
			MAPPTs->Shadow_Draw();
		}
		void Shadow_Draw(void) {
			for (auto& c : this->chara) { c.Draw_chara(); }
			for (auto& g : this->item) { g.Draw_item(); }
		}
		void Main_Draw(std::unique_ptr<MAPclass::Map, std::default_delete<MAPclass::Map>>& MAPPTs) {
			//map
			MAPPTs->main_Draw();
			//命中痕
			for (auto&s : hit_obj) { s.Draw(); }
			//サイト
			for (auto& s : get_mine().sight_) {
				s.Draw_reticle();
			}
			//キャラ
			std::for_each(this->chara.begin(), this->chara.end(), [&](Chara& c) { c.Draw_chara(); });
			//レーザー
			std::for_each(this->chara.begin(), this->chara.end(), [&](Chara& c) { c.Draw_laser(this->chara, MAPPTs, light); });
			//アイテム
			std::for_each(this->item.begin(), this->item.end(), [&](Items& i) { i.Draw_item(get_mine()); });
			//銃弾
			SetFogEnable(FALSE);
			SetUseLighting(FALSE);
			std::for_each(this->chara.begin(), this->chara.end(), [&](Chara& c) { c.Draw_ammo(); });
			SetUseLighting(TRUE);
			SetFogEnable(TRUE);
			//
		}
		void Item_Draw(void) {
			for (auto& c : this->chara) {
				c.Set_Draw_bullet();
			}
			UIparts->item_Draw(this->chara, get_mine(), get_items(), camera_main.campos, use_VR);
		}
		void LAST_Draw(void) {
			if (TPSparts->key_TPS.on()) {
				Hostpassparts_TPS->get_main().DrawGraph(0, 0, true);
			}
			//minimap
			minimapparts->Draw(64, 64);
		}
	};
};