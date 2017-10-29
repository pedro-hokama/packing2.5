//============================================================================
// Name        : cp25Packing.cpp
// Author      : Pedro Henrique Del Bianco Hokama
// Version     :
// Copyright   : Free for academics
// Description : This lib solves the packing problem with
//               rotations, using the cpoptimizer
//============================================================================


#include <vector>
#include <algorithm>
#include <limits>

#include "cp25Packing.h"
#include "ilcp/cp.h"


using namespace std;




void generate_pdf(string file, IloCP cp, int W, int H, int n, IloIntVarArray X, IloIntVarArray Y, IloIntVarArray ilow, IloIntVarArray iloh){
  ofstream tex_solution(file.c_str());
  if (!tex_solution.is_open()){
    cerr << "File not found!";
    exit(-1);
  }

  tex_solution << "\\documentclass[tikz]{standalone}" << endl;
  tex_solution << "\\usepackage{tikz}" << endl;
  tex_solution << "\\usetikzlibrary{decorations.pathreplacing}" << endl;
  tex_solution << "\\usepackage{array,siunitx}" << endl;
  tex_solution << "\\sisetup{add-decimal-zero,round-precision=2,round-mode = places}" << endl;
  tex_solution << "\\begin{document}" << endl;
  tex_solution << "\\begin{tikzpicture}[scale = 0.5]" << endl;
  tex_solution << "\\filldraw[fill=white] (0,0) rectangle (" << (double) ((double)  W / 100.0) << "," << (double) ((double)  H / 100.0) << ");" << endl;
  for(int item = 0; item < n; item++){
    tex_solution << "\\filldraw[fill=black!30!white, draw=black] ("
        << (double) ((double) cp.getValue(X[item]) / 100.0)  << "," <<  (double) ((double)  cp.getValue(Y[item]) / 100.0) << ") rectangle ("
        << (double) ((double) (cp.getValue(X[item]) + cp.getValue(ilow[item]) ) / 100.0)  << ","
        << (double) ((double) (cp.getValue(Y[item]) + cp.getValue(iloh[item]) ) / 100.0)  << ") node[pos=.5] {" << item << "};" << endl;
  }
  tex_solution << "\\end{tikzpicture}" << endl;
  tex_solution << "\\end{document}" << endl;
  tex_solution.close();
  string syscmd("pdflatex -halt-on-error -output-directory ./ ");
  syscmd.append(file);
  syscmd.append(" > /dev/null");
  if(system(syscmd.c_str()) != 0) cerr << "Something is wrong with pdflatex! Sorry.";

  return;
}

void generate_pdf(std::ostream& tex_solution, int W, int H, int n, int *w, int *h, int *p, int *x, int *y, int *l){
  double scale = (double) 10 / max(W, H);
  cout << scale << endl;

  tex_solution << "\\documentclass[tikz]{standalone}" << endl;
  tex_solution << "\\usepackage{tikz}" << endl;
  tex_solution << "\\usetikzlibrary{decorations.pathreplacing}" << endl;
  tex_solution << "\\usepackage{array,siunitx}" << endl;
  tex_solution << "\\sisetup{add-decimal-zero,round-precision=2,round-mode = places}" << endl;
  tex_solution << "\\begin{document}" << endl;
  tex_solution << "\\begin{tikzpicture}[scale = 0.5]" << endl;
  tex_solution << "\\filldraw[fill=white] (0,0) rectangle (" << (double) W * scale << "," << (double) H * scale << ");" << endl;
  for(int item = 0; item < n; item++){
    if(l[item] == 0){
      tex_solution << "\\filldraw[fill=black!" << 20 + 20*p[item] << "!white, draw=black] ("
          << (double) x[item] * scale  << "," <<  (double) y[item] * scale << ") rectangle ("
          << (double) (x[item] + w[item]) * scale  << ","
          << (double) (y[item] + h[item]) * scale  << ") node[pos=.5] {" << item << "};" << endl;
    }
  }

  tex_solution << "\\begin{scope}[shift={(10,0)}]" << endl;
  tex_solution << "\\filldraw[fill=white] (0,0) rectangle (" << (double) W * scale << "," << (double) H * scale << ");" << endl;
  for(int item = 0; item < n; item++){
    if(l[item] == 1){
      tex_solution << "\\filldraw[fill=black!" << 20 + 20*p[item] << "!white, draw=black] ("
          << (double) x[item] * scale  << "," <<  (double) y[item] * scale << ") rectangle ("
          << (double) (x[item] + w[item]) * scale  << ","
          << (double) (y[item] + h[item]) * scale  << ") node[pos=.5] {" << item << "};" << endl;
    }
  }
  tex_solution << "\\end{scope}" << endl;
  tex_solution << "\\end{tikzpicture}" << endl;
  tex_solution << "\\end{document}" << endl;


  return;
}

bool cp_25_simple_unloading(int W, int H, int n,
    int *w, int *h, int *p,
    int *x, int *y, int *l,
    double timeLimit, double *timeUsed,
    int *status, int verbose){
  IloEnv env;
  bool feasSol = false;

  try {
    IloModel model(env);
    //Model the data
    IloInt nbItens = n;
    IloInt iloW = W;
    IloInt iloH = H;

    //Variables of the problem
    IloIntVarArray X(env);
    IloIntVarArray Y(env);
    IloIntVarArray L(env);
    IloIntVarArray above(env);
    IloIntArray ilow(env, nbItens);
    IloIntArray iloh(env, nbItens);

    for (int i = 0; i < n; i++) {
      ilow[i] = w[i];
      iloh[i] = h[i];
      ostringstream vname;
      vname << "x_" << i;
      X.add(IloIntVar(env, 0, iloW - ilow[i], vname.str().c_str()));
      vname.str("");
      vname << "y_" << i;
      Y.add(IloIntVar(env, 0, iloH - iloh[i], vname.str().c_str()));
      vname.str("");
      vname << "l_" << i;
      L.add(IloIntVar(env, 0, 1, vname.str().c_str()));
      IloIntArray above_feas(env);
      above_feas.add(-1);
      for (int j = 0; j < n; j++) {
        if(i != j && p[i] <= p[j] && w[i] == w[j] && h[i] == h[j]) above_feas.add(j);
      }
      vname.str("");
      vname << "above_" << i;
      above.add(IloIntVar(env, above_feas, vname.str().c_str()));
    }

    //overlap constraint
    for (int i = 0; i < nbItens; i++) {
      model.add(X[i] + ilow[i] <= iloW);
      model.add(Y[i] + iloh[i] <= iloH);

      for (int j = i + 1; j < nbItens; j++) {
        if (p[i] < p[j]){
          model.add((X[i] + ilow[i] <= X[j])                              || (Y[j] + iloh[j] <= Y[i]) || (X[j] + ilow[j] <= X[i]) || L[j] < L[i]);
        }else if(p[j] < p[i]){
          model.add((X[i] + ilow[i] <= X[j]) || (Y[i] + iloh[i] <= Y[j])                              || (X[j] + ilow[j] <= X[i]) || L[i] < L[j]);
        }else{
          model.add((X[i] + ilow[i] <= X[j]) || (Y[i] + iloh[i] <= Y[j])  || (Y[j] + iloh[j] <= Y[i]) || (X[j] + ilow[j] <= X[i]) || L[j] != L[i]);
          //if i and j have the same size
          //if((w[i] == w[j] && h[i] == h[j]) || (w[i] == h[j] && h[i] == w[j])){
          //  model.add(X[i] <= X[j] && (IloIfThen(env,X[i] == X[j], Y[i] < Y[j])));
          //}
        }
      }

      model.add(IloIfThen(env, L[i] == 1,
(above[i] >= 0) && (X[i] == X[above[i]]) && (Y[i] == Y[above[i]])));
    }

    //Create an instance of IloCP
    IloCP cp(model);

    if(verbose < 3) cp.setParameter(IloCP::PropagationLog, IloCP::Quiet);
    cp.setParameter(IloCP::Workers, 1);
    if(verbose < 3) cp.setParameter(IloCP::LogVerbosity, IloCP::Quiet);
    if(timeLimit > 0) cp.setParameter(IloCP::TimeLimit, timeLimit);

    if(verbose > 2) cout << "Starting CP Solver" << endl;
    cp.exportModel("model.cpo");

    //Search for a solution
    if (cp.solve()) {
      for (int j = 0; j < nbItens; j++) {
        x[j] = cp.getValue(X[j]);
        y[j] = cp.getValue(Y[j]);
        l[j] = cp.getValue(L[j]);
        if(l[j] == 1){
          cout << "Item " << j << " is above item " << cp.getValue(above[j]) << endl;
        }
      }
      generate_pdf(cout, W, H, n, w, h, p, x, y, l);
      feasSol = true;

      *timeUsed = cp.getInfo(cp.SolveTime);
      *status = (int) cp.getStatus();
    } else {
      *timeUsed = cp.getInfo(cp.SolveTime);
      *status = (int) cp.getStatus();
    }
    if(verbose > 2) cout << "Finishing CP Solver" << endl;
    cp.end();
  }
  catch(IloException & e) {
    cout << "Erro" << endl;
    env.out() << " ERROR: " << e.getMessage() << std::endl;
  }

  env.end();
  return feasSol;
}


