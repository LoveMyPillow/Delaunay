#include "Delaunay.h"

const std::string psfn = R"(./pointset.dat)";


void main() {
	std::ifstream frptr;
	frptr.open(psfn, std::ios::binary);
	frptr.seekg(0, std::ios::end);
	const int filelen = frptr.tellg();
	frptr.seekg(0, std::ios::beg);
	const int pointnum = filelen / 16;

	double2* pointset = new double2[pointnum];
	frptr.read((char*)pointset, filelen);

	double2adv* pointsetadv = new double2adv[pointnum];

	for (int i = 0; i < pointnum; i++) {
		pointsetadv[i] = { i + 3, pointset[i] };
	}

	std::qsort(pointsetadv, pointnum, sizeof(double2adv), compare);

	triangle maxbound = { p1, p2, p3 };

	trileaf root = { triid, maxbound, std::vector<trileaf*>{} };

	trileaf_pointindex = new std::vector<trileaf*>[pointnum + 3];
	trileaf_pointindex[0].push_back(&root);
	trileaf_pointindex[1].push_back(&root);
	trileaf_pointindex[2].push_back(&root);

	const int power = fmax(log10(pointnum)-2, 0);
	const int unit = pow(10, power);

	for (int i = 0; i < pointnum; i++ ) {
		double2adv pointadv = pointsetadv[i];
		if(i%unit == 0)
			std::cout << i << std::endl;
		//find the triangle unit that point locates.
//		leafloop(&root);
		auto locatedleafs = pointSearch(&root, pointadv);
		trileaf* cleaf = locatedleafs[0];

		trileaf* leafs = new trileaf[3];
		edge e1, e2, e3;


		//delete the father trileaf in trileaf_pointindex
		trileaf_pointindex_delete(cleaf->t);

		//add to DAG
		triangle t1 = { cleaf->t.p1, cleaf->t.p2, pointadv };
		leafs[0].triid = ++triid;
		leafs[0].t = t1;
		cleaf->childrenptr.push_back(&leafs[0]);
		e1 = { cleaf->t.p1, cleaf->t.p2 };
		//add to trileaf_pointindex
		trileaf_pointindex[cleaf->t.p1.index].push_back(&leafs[0]);
		trileaf_pointindex[cleaf->t.p2.index].push_back(&leafs[0]);
		trileaf_pointindex[pointadv.index].push_back(&leafs[0]);
		//edge validation and legallize
		edgevalidation(&root, pointadv, e1);




		triangle t2 = { cleaf->t.p2, cleaf->t.p3, pointadv };
		leafs[1].triid = ++triid;
		leafs[1].t = t2;
		cleaf->childrenptr.push_back(&leafs[1]);
		e2 = { cleaf->t.p2, cleaf->t.p3 };
		trileaf_pointindex[cleaf->t.p2.index].push_back(&leafs[1]);
		trileaf_pointindex[cleaf->t.p3.index].push_back(&leafs[1]);
		trileaf_pointindex[pointadv.index].push_back(&leafs[1]);
		edgevalidation(&root, pointadv, e2);


		triangle t3 = { cleaf->t.p1, cleaf->t.p3, pointadv };
		leafs[2].triid = ++triid;
		leafs[2].t = t3;
		cleaf->childrenptr.push_back(&leafs[2]);
		e3 = { cleaf->t.p1, cleaf->t.p3 };
		trileaf_pointindex[cleaf->t.p1.index].push_back(&leafs[2]);
		trileaf_pointindex[cleaf->t.p3.index].push_back(&leafs[2]);
		trileaf_pointindex[pointadv.index].push_back(&leafs[2]);
		edgevalidation(&root, pointadv, e3);
	}

	leafloop(pointnum);
}