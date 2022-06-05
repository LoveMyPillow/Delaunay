// copyright@Huan_Xu
// Delaunay triangulation using incremental
// any question please email me: hiramhsu404@gmail.com
// Author: Huan Xu
// Date: 2022/06/04

#pragma once
#include <vector>
#include <queue>
#include <exception>
#include <algorithm>
#include <unordered_set>

int triid = 0;

struct double2
{
	double x, y;
};

struct double2adv {
	int index;
	double2 loc;
};

struct edge {
	double2adv p1, p2;
};

bool operator > (const double2& a, const double2& b) {
	return (a.x > b.x) || (a.x == b.x && a.y > b.y);
}

bool operator == (const double2& a, const double2& b) {
	return (a.x == b.x) && (a.y == b.y);
}

bool operator != (const double2& a, const double2& b) {
	return !((a.x == b.x) && (a.y == b.y));
}

bool operator < (const double2& a, const double2& b) {
	return (a.x < b.x) || (a.x == b.x && a.y < b.y);
}

bool operator == (const edge& a, const edge& b) {
	return (a.p1.index == b.p1.index && a.p2.index == b.p2.index) || (a.p1.index == b.p2.index && a.p2.index == b.p1.index);
}

bool operator != (const edge& a, const edge& b) {
	return !((a.p1.index == b.p1.index && a.p2.index == b.p2.index) || (a.p1.index == b.p2.index && a.p2.index == b.p1.index));
}

int compare(const void* elem1, const void* elem2) {
	double2 a = *(double2*)elem1;
	double2 b = *(double2*)elem2;

	if (a > b) return 1;
	else if (a == b) return 0;
	else return -1;
}


struct triangle {
	double2adv p1, p2, p3;

	double2adv* counterclockwise() {
		double2adv* pcollect = new double2adv[3];
		pcollect[0] = p1;
		pcollect[1] = p2;
		pcollect[2] = p3;
		std::qsort(pcollect, 3, sizeof(double2adv), compare);
		double dy1 = pcollect[1].loc.y - pcollect[0].loc.y;
		double dx1 = pcollect[1].loc.x - pcollect[0].loc.x;
		if (dx1 == 0) {
			//if 1 and 0 are on same vertical line , it should be 0 , 2, 1
			double2adv temp = pcollect[1];
			pcollect[1] = pcollect[2];
			pcollect[2] = temp;
			return pcollect;
		}
		double dy2 = pcollect[2].loc.y - pcollect[0].loc.y;
		double dx2 = pcollect[2].loc.x - pcollect[0].loc.x;
		if (dx2 == 0) {
			return pcollect;
		}
		if (dy2 / dx2 < dy1 / dx1) {
			// if 2 has a small skew than 1, it should be 0, 2, 1
			double2adv temp = pcollect[1];
			pcollect[1] = pcollect[2];
			pcollect[2] = temp;
			return pcollect;
		}
		else {
			return pcollect;
		}
	};
};

struct trileaf
{
	int triid;
	triangle t;
	std::vector<trileaf*> childrenptr;

	//if point is a vertex of t;
	bool isvertex(const double2adv& point) const {
		return  t.p1.index == point.index || t.p2.index == point.index || t.p3.index == point.index;
	}

	//if any vertex of t has a index "id";
	bool isvertex(const int& id) const {
		return t.p1.index == id || t.p2.index == id || t.p3.index == id;
	}
};

std::vector<trileaf*>* trileaf_pointindex;

//return all trileaves which contain expected edge in trileaf_pointindex
std::vector<trileaf*> edgesearch(const edge& e) {
	std::vector<trileaf*> newleafs;
	int i = 0;
	auto trileafswithp1 = trileaf_pointindex[e.p1.index];
	if (trileafswithp1.size()) {
		for (auto trileafwithp1 : trileafswithp1) {
			if (trileafwithp1->isvertex(e.p2)) {
				newleafs.push_back(trileafwithp1);
				++i;
			}
			if (i == 2) {
				break;
			}
		}
	}
	return newleafs;
}

//delete a expected trileaf in trileaf_pointindex
void trileaf_pointindex_delete(const triangle& t) {
	int a = t.p1.index;
	int b = t.p2.index;
	int c = t.p3.index;
	//there are three duplicated triangle indexed by three vertices
	//first
	std::vector<trileaf*>& trileafswithp1 = trileaf_pointindex[a];
	if (trileafswithp1.size()) {
		for (int i = 0; i < trileafswithp1.size(); i++) {
			auto trileafwithp1 = trileafswithp1[i];
			if (trileafwithp1->isvertex(b) && trileafwithp1->isvertex(c)) {
				trileafswithp1.erase(trileafswithp1.begin() + i);
				break;//there is only one expected triangle in each index.
			}
		}
	}
	//second
	std::vector<trileaf*>& trileafswithp2 = trileaf_pointindex[b];
	if (trileafswithp2.size()) {
		for (int i = 0; i < trileafswithp2.size(); i++) {
			auto trileafwithp2 = trileafswithp2[i];
			if (trileafwithp2->isvertex(a) && trileafwithp2->isvertex(c)) {
				trileafswithp2.erase(trileafswithp2.begin() + i);
				break;
			}
		}
	}
	//third
	std::vector<trileaf*>& trileafswithp3 = trileaf_pointindex[c];
	if (trileafswithp3.size()) {
		for (int i = 0; i < trileafswithp3.size(); i++) {
			auto trileafwithp3 = trileafswithp3[i];
			if (trileafwithp3->isvertex(a) && trileafwithp3->isvertex(b)) {
				trileafswithp3.erase(trileafswithp3.begin() + i);
				break;
			}
		}
	}
}

//delete all trileaves which contain expected edge in trileaf_pointindex
void trileaf_pointindex_delete(const edge& e) {
	int a = e.p1.index;
	int b = e.p2.index;

	std::vector<triangle> deleteindex;
	std::vector<trileaf*>& trileafswithp1 = trileaf_pointindex[a];
	if (trileafswithp1.size()) {
		for (int i = 0; i < trileafswithp1.size(); i++) {
			auto trileafwithp1 = trileafswithp1[i];
			if (trileafwithp1->isvertex(b)) {
				deleteindex.push_back(trileafwithp1->t);
			}
		}
	}
	for (const triangle& i : deleteindex) {
		trileaf_pointindex_delete(i);
	}
}


int hit(const triangle& s, const double2adv& p) {
	double x1, x2, x3, y1, y2, y3, x, y;
	x1 = s.p1.loc.x;
	x2 = s.p2.loc.x;
	x3 = s.p3.loc.x;
	y1 = s.p1.loc.y;
	y2 = s.p2.loc.y;
	y3 = s.p3.loc.y;
	x = p.loc.x;
	y = p.loc.y;

	double deno = (y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3);
	double lambda1 = ((y2 - y3) * (x - x3) + (x3 - x2) * (y - y3)) / deno;
	double lambda2 = ((y3 - y1) * (x - x3) + (x1 - x3) * (y - y3)) / deno;
	if (lambda1 > 0 && lambda2 > 0 && (lambda1 + lambda2 < 1)) {
		return 1;
	}
	else if (lambda1 < 0 || lambda2 < 0 || lambda1 + lambda2 > 1) {
		return -1;
	}
	else {
		return 0;
	}
}

std::vector<trileaf*> pointSearch(trileaf* root, double2adv point) {
	std::vector<trileaf*> leafs;
	std::queue<trileaf*> searchqueue;
	searchqueue.push(root);

	while (!searchqueue.empty())
	{
		trileaf* cleaf = searchqueue.front();
		searchqueue.pop();

		int hitresult = hit(cleaf->t, point);
		if (hitresult > 0 || hitresult == 0) {
			if (cleaf->childrenptr.size()) {
				for (auto childptr : cleaf->childrenptr) {
					searchqueue.push(childptr);
				}
			}
			else {
				leafs.push_back(cleaf);
				break;
			}
		}
	}
	return leafs;
}


const double2adv p1 = { 0, 1500,1000 };
const double2adv p2 = { 1, -2500, 1000 };
const double2adv p3 = { 2, 0, -1500 };
const edge edge1 = { p1, p2 };
const edge edge2 = { p1, p3 };
const edge edge3 = { p2,p3 };

bool isboundaryvertex(const double2adv& p) {
	return p.index == p1.index || p.index == p2.index || p.index == p3.index;
}

void edgevalidation(trileaf* root, const double2adv& newpoint, const edge& keyedge) {
	if (keyedge != edge1 && keyedge != edge2 && keyedge != edge3) {
		auto leafs = edgesearch(keyedge);
		bool i = leafs[0]->isvertex(newpoint);
		bool j = leafs[1]->isvertex(newpoint);
		trileaf* existedleaf;

		if (i == false && j == true) {
			existedleaf = leafs[0];
		}
		else if (i == true && j == false) {
			existedleaf = leafs[1];
		}
		else {
			throw new std::exception();
		}
		auto tri1 = existedleaf->t.counterclockwise();


		int counter = 0;
		double judge = 0;
		if (isboundaryvertex(existedleaf->t.p1))counter++;
		if (isboundaryvertex(existedleaf->t.p2))counter++;
		if (isboundaryvertex(existedleaf->t.p3))counter++;

		double2adv commonpointnew;
		if (existedleaf->t.p1.index != keyedge.p1.index && existedleaf->t.p1.index != keyedge.p2.index) {
			commonpointnew = existedleaf->t.p1;
		}
		else if (existedleaf->t.p2.index != keyedge.p1.index && existedleaf->t.p2.index != keyedge.p2.index) {
			commonpointnew = existedleaf->t.p2;
		}
		else {
			commonpointnew = existedleaf->t.p3;
		}


		//find if these two triangle forms a concave polygon
		bool isconcave = true;
		double2 P1 = keyedge.p1.loc;
		double2 P2 = keyedge.p2.loc;
		double2 Q1 = newpoint.loc;
		double2 Q2 = commonpointnew.loc;
		//fast judge
		if (fmin(P1.x, P2.x) <= fmax(Q1.x, Q2.x) &&
			fmin(Q1.x, Q2.x) <= fmax(P1.x, P2.x) &&
			fmin(P1.y, P2.y) <= fmax(Q1.y, Q2.y) &&
			fmin(Q1.y, Q2.y) <= fmax(P1.y, P2.y)
			) {
			//cross judge
			if (((Q1.x - P1.x) * (Q1.y - Q2.y) - (Q1.y - P1.y) * (Q1.x - Q2.x)) * ((Q1.x - P2.x) * (Q1.y - Q2.y) - (Q1.y - P2.y) * (Q1.x - Q2.x)) < 0 &&
				((P1.x - Q1.x) * (P1.y - P2.y) - (P1.y - Q1.y) * (P1.x - P2.x)) * ((P1.x - Q2.x) * (P1.y - P2.y) - (P1.y - Q2.y) * (P1.x - P2.x)) < 0) {
				isconcave = false;
			}
		}



		if (isconcave) {
			judge = 0;
		}
		else if (counter == 1) {
			if (isboundaryvertex(keyedge.p1) || isboundaryvertex(keyedge.p2)) {
				judge = 1;
			}
		}
		else {
			double judgematix[3][3];
			judgematix[0][0] = (tri1[0].loc.x - newpoint.loc.x);
			judgematix[0][1] = (tri1[0].loc.y - newpoint.loc.y);
			judgematix[0][2] = (tri1[0].loc.x * tri1[0].loc.x - newpoint.loc.x * newpoint.loc.x) +
				(tri1[0].loc.y * tri1[0].loc.y - newpoint.loc.y * newpoint.loc.y);
			judgematix[1][0] = (tri1[1].loc.x - newpoint.loc.x);
			judgematix[1][1] = (tri1[1].loc.y - newpoint.loc.y);
			judgematix[1][2] = (tri1[1].loc.x * tri1[1].loc.x - newpoint.loc.x * newpoint.loc.x) +
				(tri1[1].loc.y * tri1[1].loc.y - newpoint.loc.y * newpoint.loc.y);
			judgematix[2][0] = (tri1[2].loc.x - newpoint.loc.x);
			judgematix[2][1] = (tri1[2].loc.y - newpoint.loc.y);
			judgematix[2][2] = (tri1[2].loc.x * tri1[2].loc.x - newpoint.loc.x * newpoint.loc.x) +
				(tri1[2].loc.y * tri1[2].loc.y - newpoint.loc.y * newpoint.loc.y);

			judge += judgematix[0][0] *
				(judgematix[1][1] * judgematix[2][2] - judgematix[2][1] * judgematix[1][2]);
			judge -= judgematix[0][1] *
				(judgematix[1][0] * judgematix[2][2] - judgematix[1][2] * judgematix[2][0]);
			judge += judgematix[0][2] *
				(judgematix[1][0] * judgematix[2][1] - judgematix[1][1] * judgematix[2][0]);
		}

		if (judge > 0) {
			//it means it is a illegal edge

			//step1. delete triangles contain this edge
			trileaf_pointindex_delete(keyedge);

			//step2. add new two flipped triangle to DAG
			//and add new two flipped triangle to trileaf_pointindex
			trileaf* newleaf = new trileaf[2];
			newleaf[0].triid = ++triid;
			newleaf[1].triid = ++triid;
			newleaf[0].t = { newpoint, commonpointnew, keyedge.p1 };
			newleaf[1].t = { newpoint, commonpointnew, keyedge.p2 };
			leafs[0]->childrenptr.push_back(&newleaf[0]);
			leafs[0]->childrenptr.push_back(&newleaf[1]);
			leafs[1]->childrenptr.push_back(&newleaf[0]);
			leafs[1]->childrenptr.push_back(&newleaf[1]);
			trileaf_pointindex[newpoint.index].push_back(&newleaf[0]);
			trileaf_pointindex[newpoint.index].push_back(&newleaf[1]);
			trileaf_pointindex[commonpointnew.index].push_back(&newleaf[0]);
			trileaf_pointindex[commonpointnew.index].push_back(&newleaf[1]);
			trileaf_pointindex[keyedge.p1.index].push_back(&newleaf[0]);
			trileaf_pointindex[keyedge.p2.index].push_back(&newleaf[1]);


			edgevalidation(root, newpoint, { commonpointnew, keyedge.p1 });
			edgevalidation(root, newpoint, { commonpointnew, keyedge.p2 });
		}
		else {
			//it means it is a legal edge
			return;
		}
	}
	else
	{
		return;
	}
}

void leafloop(int points) {
	std::cout << "=====================================================" << std::endl;
	for (int i = 0; i < points; i++) {
		auto cvec = trileaf_pointindex[i + 3];

		for (const trileaf* cleaf : cvec) {
			if (cleaf->isvertex(0) || cleaf->isvertex(1) || cleaf->isvertex(2)) {
			}
			else {
				std::cout << "(*" << cleaf->triid << "*)" << " Triangle[{{" << cleaf->t.p1.loc.x << ", " << cleaf->t.p1.loc.y
					<< "}, {" << cleaf->t.p2.loc.x << " ," << cleaf->t.p2.loc.y
					<< " }, {" << cleaf->t.p3.loc.x << " ," << cleaf->t.p3.loc.y << "}}]," << std::endl;
				//			std::cout << std::endl;
			}
		}
	}
}