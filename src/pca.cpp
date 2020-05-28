#include "pca.h"
#include "common.h"

PCA::PCA(int m_, string data_path_, string write_path_, bool small)
    :m(m_), data_path(data_path_), write_path(write_path_)
{
    if(small){
        template_path = s_template_path;
        n = 2319;
    }else{
        template_path = l_template_path;
        n = 22779;
    }

    igl::read_triangle_mesh(template_path, V_template, F_template);
    load_faces();
    cout << "Found " << nFiles << " faces.\n";
    Eigen::MatrixXd V_temp;
    Eigen::MatrixXi F_temp;
    Eigen::VectorXd happy, neutral;
    igl::read_triangle_mesh(happy_path, V_temp, F_temp);
    Eigen::MatrixXd Vt = V_temp.transpose();
    happy = Eigen::Map<Eigen::VectorXd>(Vt.data(), Vt.size());
    igl::read_triangle_mesh(neutral_path, V_temp, F_temp);
    Vt = V_temp.transpose();
    neutral = Eigen::Map<Eigen::VectorXd>(Vt.data(), Vt.size());
    expression = happy - neutral;
}

void PCA::load_template(){
    this->V = V_template;
    this->F = F_template;
}

void PCA::get_template(Eigen::MatrixXd &V_ext, Eigen::MatrixXi &F_ext){
    V_ext = V_template;
    F_ext = F_template;
}

void PCA::load_faces(){
    string file_path;
    Eigen::MatrixXd V_temp;
    Eigen::MatrixXd F_temp;
    string ext = ".obj";

    findFilesWithExt(data_path, ext, listOfFiles);
    nFiles = listOfFiles.size();
    Faces.setZero(nFiles, 3*n);
    for(int i = 0; i < nFiles; i++){
        file_path = data_path + "/" + listOfFiles[i];
        cout << listOfFiles[i] << endl;
        igl::read_triangle_mesh(file_path, V_temp, F_temp);
        V_temp.transposeInPlace();
        Faces.row(i) = Eigen::Map<Eigen::RowVectorXd>(V_temp.data(), V_temp.size());
        V_temp.resize(0,3);
    }
}

void PCA::compute_pca()
{
    mF = Faces.colwise().mean();
    reshape(mF, mF.size()/3, 3, M);
    if(nFiles < m){
        cout << "Not enough faces in database, choose smaller m, n = "<< n << ", m = "<<m<<endl;
        m = nFiles;
    }
        
    auto Faces_centered = Faces.rowwise() - mF.transpose();
    S = Faces_centered * Faces_centered.adjoint();
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigen(S);
    auto v = eigen.eigenvectors();
    Eigen::MatrixXd u = Faces.adjoint() * v; 
    eigenvectors = u.block(0, nFiles - m, 3*n, m); //dxm matrix of eigenvectors (in cols)
    eigenvectors.colwise().normalize(); // normalize
    eigenvalues = eigen.eigenvalues().segment(nFiles - m, m).colwise().normalized();
}   


void PCA::eigenface(const Eigen::VectorXd &slider, Eigen::VectorXd &base){
    Eigen::VectorXd weights;
    weights = slider;
    Eigen::VectorXd face;

    if(weights.hasNaN()){
        weights = Eigen::VectorXd::Zero(m);
    }    

    face = base + eigenvectors * weights;
    reshape(face.transpose(), face.size()/3, 3, V);
}


void PCA::random_face(){
    std::normal_distribution<double> dist(0.0, variance);
    Eigen::VectorXd face, random_weights;
    random_weights.resize(m);
    face = mF;
    for(int i = 0; i < m;i++){
        random_weights[i] = dist(eng) / sqrt(eigenvalues[i]);
    }
    eigenface(random_weights, mF);
}

void PCA::morphface(Eigen::VectorXd &f1, Eigen::VectorXd &f2, Eigen::VectorXd &w1, Eigen::VectorXd &w2, double p){
    Eigen::VectorXd face;
    Eigen::VectorXd weights;
    weights = w1 - p*(w1 - w2);
    eigenface(weights, mF);
}

void PCA::change_expression(double p, Eigen::VectorXd &base){
    Eigen::VectorXd face;
    face = base + p * expression;
    reshape(face.transpose(), face.size()/3, 3, V);
}

void PCA::compute_weights(Eigen::VectorXd &face, Eigen::VectorXd &weights){
    weights.setZero(m);
    Eigen::VectorXd diff = face - mF;
    weights = eigenvectors.transpose() * diff;
}

void PCA::reconstruct(Eigen::VectorXd &face){
    Eigen::VectorXd weights;
    compute_weights(face, weights);
    eigenface(weights, mF);
}

void PCA::read_face(const string filename, Eigen::VectorXd &face){
    Eigen::MatrixXd V_t;
    Eigen::MatrixXi F_t;
    igl::read_triangle_mesh(filename, V_t, F_t);
    V_t.transposeInPlace();
    face = Eigen::Map<Eigen::VectorXd>(V_t.data(), V_t.size());
    cout << face.segment(0, 10).transpose() << endl;
}