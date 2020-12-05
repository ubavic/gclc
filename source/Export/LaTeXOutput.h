// LaTeXOutput.h: interface for the CLaTeXOutput class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(LATEXOUTPUT_H)
#define LATEXOUTPUT_H

#include "GCLCOutput.h"

class CLaTeXOutput : public CGCLCOutput {
public:
  CLaTeXOutput(ofstream &h);
  virtual ~CLaTeXOutput();

  GReturnValue FillTriangle(double, double, double, double, double, double) {
    return rvG_OK;
  }
  GReturnValue FillRectangle(double, double, double, double) { return rvG_OK; }
  GReturnValue FillEllipseArc(double, double, double, double, double, double,
                              bool) {
    return rvG_OK;
  }

  void Reset();
  GReturnValue Init(int x, int y);
  GReturnValue Close();
  GReturnValue SetPixel(double x, double y);
  GReturnValue DrawSegment(double x1, double y1, double x2, double y2);
  GReturnValue DrawArc(double x1, double y1, double x2, double y2, double phi,
                       int precision);
  GReturnValue SetColor(unsigned char r, unsigned char g, unsigned char b);
  GReturnValue SetBackgroundColor(unsigned char r, unsigned char g,
                                  unsigned char b);
  GReturnValue SetThickness(double uThickness);
  GReturnValue PrintText(double x1, double y1, const string &sPosition,
                         const string &sText);
  GReturnValue PrintComment(const string &sText);
  GReturnValue Export(const string &sText, export_type format);
  GReturnValue SetFontSize(unsigned char size);
  void SetPointCounter(int counter) { COUNTER = counter; }
  int GetPointCounter() { return COUNTER; }
  virtual void SetCirclePrecision(int precision) { PRECISION = precision; }

private:
  void PrintToOutput(const string &s);
  int OUTPUT_LINE, COUNTER, PRECISION;
  double LINE_WIDTH;
  ofstream &OUTPUT;
  double m_dCurrentLineWidth;
  bool m_bColorsUsed;
};

#endif // !defined(LATEXOUTPUT_H)
