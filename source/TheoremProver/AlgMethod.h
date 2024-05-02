// AlgMethod.h: interface for the CAlgMethod class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(ALGMETHOG_H)
#define ALGMETHOG_H

#include "../AlgebraicMethods/Prover.h"
#include "TheoremProver.h"
#include "stdarg.h"
#include <memory>

//
// Constant
//
struct Constant {
  // index of free variable
  int Index;

  // name
  std::string Name;

  Constant(const std::string &name, int index) {
    Name = name;
    Index = index;
  }
};

//
// struct HalfPoint
// represents one coordinate of the point
//
struct HalfPoint {
  bool Free;
  int Index;
  bool Determined;
#ifdef GCLC_COORDINATES
  int Coordinate; // disable usage of coordinate
#endif

  HalfPoint(bool free,
#ifdef GCLC_COORDINATES
            int coordinate = 0,
#endif
            int index = 0, bool determined = false) {
    Free = free;
    Index = index;
    Determined = determined;
#ifdef GCLC_COORDINATES
    Coordinate = coordinate;
#endif
  }
};

//
// struct Point
// is a point of the construction
//
struct Point {
  HalfPoint X;
  HalfPoint Y;
  std::string Name;

  Point(const Point &p) : X(p.X.Free), Y(p.Y.Free) { Name = p.Name; }

  Point(bool xFree, bool yFree, const std::string &name) : X(xFree), Y(yFree) {
    Name = name;
  }
};

//
// Line structure
//
struct Line {
  Point *p1;    // first point on line, or NULL
  Point *p2;    // second point on line, or NULL
  std::string Name;  // line name
  double Angle; // angle between line and X axis, negative value means unknown
  std::vector<Point *> Points; // points on the line
  bool implicit;          // implicit line is line generated by the prover

  Line(const std::string &name) : p1(NULL), p2(NULL), Angle(-1) {
    Name = name;
    implicit = false;
  }
  ~Line() {}

  // put point on line
  // only if line is not yet resolved
  void AddPoint(Point *p) {
    if (p == NULL) {
      // point does not exists
      return;
    }
    if (p1 == NULL) {
      p1 = p;
    } else if (p2 == NULL && p1->Name != p->Name) {
      p2 = p;
    }

    // add point to the list
    bool containPoint = false;
    for (int ii = 0, size = Points.size(); ii < size && !containPoint; ii++) {
      containPoint = Points[ii] == p;
    }
    if (!containPoint) {
      Points.push_back(p);
    }
  }

  bool Resolved() const { return (p1 != NULL && p2 != NULL); }
};

//
// Circle structure
//
struct Circle {
  Point *c;
  Point *p;
  std::string Name;

  Circle(const std::string &name, Point *cc, Point *pp) : c(cc), p(pp) {
    Name = name;
  }
};

//
// Conic structure
// Conic is given with five parameters (halfpoints)
//
struct Conic {
  HalfPoint H1, H2, H3, H4, H5;
  std::string Name;

  Conic(const std::string &name) : H1(true), H2(true), H3(true), H4(true), H5(true) {
    Name = name;
  }
};

class CAlgMethod : public CTheoremProver {
private:
  uint _maxt;
  int lastFreeIndex;
  int freePointCoordinatesCount;
  int lastDepIndex;

  // construction point
  std::vector<Point *> _points;
  bool _pointsResolved;

  // construction lines
  std::vector<Line *> _lines;
  bool _linesResolved;

  // construction circles
  std::vector<std::unique_ptr<Circle>> _circles;

  // construction conics
  std::vector<std::unique_ptr<Conic>> _conics;

  // constants
  std::vector<Constant *> _constants;

  // polynomial system
  vxp vxps;

  // print points coordinates into output file
  bool _printPoints;

  // conjectures system
  vxp vxpConjectures;
  // all these vectors has same number of elements!
  std::vector<CGCLCProverExpression> vpConjectures;
  std::vector<std::string> vsConjectures; // gclc conjecture string
  // original conjectures (in case they have changed)
  std::vector<CGCLCProverExpression> vpOrigConjectures;

  // initialize prover
  void _Initialize();

  void _PrintCommand(CGCLCProverCommand &Command);

  bool _FindPoints();
  bool _AddFreePoint(const std::string &name, bool addCommand, const std::string &xc,
                     const std::string &yc);
  bool _AddDependantPoint(const std::string &name, int *pLastDepIndex,
                          bool addCommand = false);
  Point *_FindPoint(const std::string &name);
  Point *_FindOrAddPoint(const std::string &name);
  Line *_FindLine(const std::string &name);
  Line *_FindLine(Point *p1, Point *p2);
  Circle *_FindCircle(const std::string &name);
  Conic *_FindConic(const std::string &name);
  bool _FindLinesCircles();
  bool _ExistsPoint(const std::string &name);
  void _PrintPoints();
  void _PrintPoint(Point *p);
  void _PrintLine(Line &l);
  void _PrintCircle(Circle &c);
  void _PrintConic(Conic &c);
  Constant *_FindConstant(const std::string &name);
  Line *_CreateLine(Point *p1, Point *p2);
  Circle *_CreateCircle(Point *p1, Point *p2);

  void _ReducibilityLineCircleIntersection(Point *p1, Point *p2, Line *l,
                                           Circle *k, Circle *k1);
  bool _UsedPoint(Point *p, std::list<CGCLCProverCommand>::iterator commandSkip);
  bool _UsedPointInConjecture(Point *p);
  void _FindParallelLines();
  void _FindParallelLines(bool xAxis);
  bool _HalfPointsEquals(Point *p1, Point *p2, bool xAxis);
  bool _OnTheSameLine(Point *p, Point *q, Point *r);

  void _ExtractConditionPolynomials();
  void _ExtractConjecturePolynomial();
  XPolynomial *_ExtractPolynomialExpression(CGCLCProverExpression *e);
  bool _CollinearPoints(Point *a, Point *b, Point *c);

  CGCLCProverExpression *_CloneExpression(CGCLCProverExpression *e);
  bool _RationalizeConjecture(CGCLCProverExpression *conjecture);
  bool _FindRationalExpression(const CGCLCProverExpression &e,
                               CGCLCProverExpression &ratExp);
  void _PropagateRationalExpression(CGCLCProverExpression *conjecture,
                                    CGCLCProverExpression *e,
                                    CGCLCProverExpression *e1, int index);
  void _PrintConjecture(const CGCLCProverExpression &e, int level, int space);

  // conditions
  void _AddCondition(XPolynomial *xp, bool check = true,
                     vxp *polySystem = NULL);
  XPolynomial *_SegmentSize(Point *p, Point *q);
  XPolynomial *_EqualTangensCondition(Line *l, Point *b, Point *a, Point *c);
  XPolynomial *_EqualTangensCondition(Point *b1, Point *a1, Point *c1,
                                      Point *b2, Point *a2, Point *c2,
                                      bool print = false);
  XPolynomial *_TangensNum(Point *b, Point *a, Point *c);
  XPolynomial *_TangensDen(Point *b, Point *a, Point *c);
  XPolynomial *_PointOnLineCondition(Point *p, Point *q1, Point *q2);
  XPolynomial *_MidpointCondition(Point *p, Point *q1, Point *q2, bool bX);
  XPolynomial *_DiffRatioCondition(Point *p1, Point *p2, Point *p3, Point *p4,
                                   Line *l, const std::string &sr);
  XPolynomial *_DiffRatioCondition(HalfPoint *h1, HalfPoint *h2, HalfPoint *h3,
                                   HalfPoint *h4, REAL r);
  XPolynomial *_DiffPoints(HalfPoint *h1, HalfPoint *h2);
  XPolynomial *_IdenticalCondition(Point *p, Point *q);
  XPolynomial *_AlgSumZero3Expression(Point *a, Point *b, Point *c, Point *d,
                                      Point *e, Point *f);
  XPolynomial *_AlgSumZero3Expression(XPolynomial *xc1, XPolynomial *xc2,
                                      XPolynomial *xc3);
  XPolynomial *_AddSegmentLengthConditionVariable(XPolynomial *xp);
  XPolynomial *_SumOfTerms(XPolynomial *x1, XPolynomial *x2, XPolynomial *x3,
                           int z1, int z2, int z3);
  XPolynomial *_PerpendicularCondition(Point *p1, Point *p2, Point *q1,
                                       Point *q2);
  XPolynomial *_EqualSegmentCondition(Point *p1, Point *p2, Point *q1,
                                      Point *q2);
  XPolynomial *_PointOnConicCondition(Conic *h, Point *p);
  XPolynomial *_ParallelCondition(Point *p1, Point *p2, Point *q1, Point *q2);
  XPolynomial *_HelperCondition(HalfPoint *h1, HalfPoint *h2, HalfPoint *h3,
                                HalfPoint *h4, HalfPoint *h5, HalfPoint *h6,
                                HalfPoint *h7, HalfPoint *h8,
                                bool print = true);
  XPolynomial *_HalfHelperCondition(HalfPoint *h1, HalfPoint *h2, HalfPoint *h3,
                                    HalfPoint *h4);

  void PrintProofFooter(eGCLC_conjecture_status eRet);

public:
  CAlgMethod();
  virtual ~CAlgMethod();
  void CleanUp();

  bool AddProverCommand(eGCLC_prover_command type, const std::string &a1 = "",
                        const std::string &a2 = "", const std::string &a3 = "",
                        const std::string &a4 = "", const std::string &a5 = "");
  bool SetProverConjecture(const std::string &strConjecture);

  void SetPrintPoints(bool printPoints);
  void PrintMethodSpecificOutput();
  std::string GetMethodSpecificOutput();
  void PrintProofTitleXML(const std::string &theoremName);

protected:
  Prover *_prover;

  enum eGCLC_conjecture_status
  ProveConjecture(const CGCLCProverExpression &Conj);
  enum eGCLC_conjecture_status _Prove(const CGCLCProverExpression &Conj);
};

#endif // !defined(ALGMETHOG_H)
