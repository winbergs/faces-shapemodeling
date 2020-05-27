
#include <igl/opengl/glfw/Viewer.h>
#include <igl/opengl/glfw/imgui/ImGuiMenu.h>
#include <igl/opengl/glfw/imgui/ImGuiHelpers.h>
#include <imgui/imgui.h>

#include "../src/pca.h"

#include <string>
#include <map>
#include <vector>

using namespace std;

using Viewer = igl::opengl::glfw::Viewer;

int m = 10;
bool s = true;

string data_path = "../data/aligned_faces_example/default";
string write_path = "./";
string write_file = "default.obj";
string inputFile = "../data/face_template/template_small.obj";
vector<double> slider;
double minS = -1.0;
double maxS = 1.0;
double zero = 0.0;
Eigen::VectorXd base;

Viewer viewer;
std::shared_ptr<PCA> pca;

/////////////////////////////GUI////////////////////////////////////////
bool callback_key_down(Viewer &viewer, unsigned char key, int modifiers);
bool callback_mouse_down(Viewer &viewer, int button, int modifier);
bool callback_mouse_move(Viewer &viewer, int mouse_x, int mouse_y);
bool callback_mouse_up(Viewer &viewer, int button, int modifier);
bool callback_pre_draw(Viewer &viewer);
bool callback_key_pressed(Viewer &viewer, unsigned char key, int modifiers);
bool draw_viewer_menu();
bool reload();
///////////////////////////////////////////////////////////////////////////


bool draw_viewer_menu(){
    
    if (ImGui::CollapsingHeader("Parameters", ImGuiTreeNodeFlags_DefaultOpen))
    {
        bool change = false;
        for(int i = 0; i < m; i++){
            string slider_name = "Eigenvector " + to_string(i);
            if(ImGui::SliderScalar(slider_name.c_str(), ImGuiDataType_Double, &(slider[m-(i+1)]), &minS, &maxS)){
                change = true;
            }
        }
        if(change){
            pca->morph_face(slider, base);
            reload();
        }
    }

    ImGui::SliderScalar("var", ImGuiDataType_Double, &(pca->variance), &zero, &maxS);

    if(ImGui::Button("Reset weights")){
        slider = vector<double>(m, 0.0);
		pca->morph_face(slider, base);
		reload();		
    }

    if(ImGui::Button("Write to File")){
        string full_write_path = write_path + write_file;
        cout << "Written tocd " << full_write_path << endl;
        igl::writeOBJ(full_write_path, pca->V, pca->F);
    }

    ImGui::PushItemWidth(-40);
    ImGui::PushID("Output File Name");
    ImGui::InputText("", write_file);
    ImGui::PopID();
    ImGui::PopItemWidth();


    ImGui::PushItemWidth(-40);
    ImGui::PushID("Input File##Landmark");

    ImGui::InputText("", inputFile);
    ImGui::PopID();
    ImGui::PopItemWidth();

    if (ImGui::CollapsingHeader("Load Base", ImGuiTreeNodeFlags_DefaultOpen))
    {
        float w = ImGui::GetContentRegionAvailWidth();
        float p = ImGui::GetStyle().FramePadding.x;
        if (ImGui::Button("Load##Mesh", ImVec2((w-p), 0)))
        {
            std::string fname = igl::file_dialog_open();
            igl::read_triangle_mesh(fname, pca->V, pca->F);
            base = Eigen::Map<Eigen::VectorXd>(pca->V.transpose().data(), pca->V.size()) - pca->mF;
            reload();
        }
    }

}
bool callback_key_down(Viewer &viewer, unsigned char key, int modifiers){
    return true;
}

bool callback_mouse_down(Viewer &viewer, int button, int modifier){

    return true;
}

bool callback_mouse_move(Viewer &viewer, int mouse_x, int mouse_y){
    return true;
}

bool callback_mouse_up(Viewer &viewer, int button, int modifier){
    pca->morph_face(slider, base);
    reload();
    return true;
}

bool callback_key_pressed(Viewer &viewer, unsigned char key, int modifiers){
	switch (key) {
	case '1':
        pca->morph_face(slider, base);
        reload();
		break;
    case '2':
        pca->V = pca->M;
        base = pca->mF;
        reload();
		break;
	case 'R':
		pca->random_face();
		reload();
		break;
	}
    
    return true;
}

bool reload(){
	viewer.data().clear();
	viewer.data().set_mesh(pca->V, pca->F);
	viewer.core.align_camera_center(pca->V);
	return true;
}

int main(int argc,char *argv[]){
    int opt;
	while((opt = getopt(argc, argv, "m:d:w:s:")) != -1){
		switch(opt){
			case 'm':
				m = atoi(optarg);
				break;
            case 'd':
                data_path = optarg;
                break;
            case 'w':
                write_path = optarg;
                break;
            case 's':
                int small = atoi(optarg);
                s = small > 0;
                break;
		}
	} 
    slider = vector<double>(m, 0.0);
    pca = std::shared_ptr<PCA>(new PCA(m, data_path, write_path, s));
    pca->load_template();
    reload();
    pca->compute_pca();
	base = pca->mF;
    cout << "computed pca" << endl;
	igl::opengl::glfw::imgui::ImGuiMenu menu;
	viewer.plugins.push_back(&menu);

	menu.callback_draw_viewer_menu = [&]()
	{
        // menu.draw_viewer_menu();
        draw_viewer_menu();
	};

    // viewer.callback_key_down = callback_key_down;
    // viewer.callback_mouse_down = callback_mouse_down;
    // viewer.callback_mouse_move = callback_mouse_move;
    // viewer.callback_mouse_up = callback_mouse_up;
	viewer.callback_key_pressed = callback_key_pressed;

	viewer.launch();

    return 0;
}
