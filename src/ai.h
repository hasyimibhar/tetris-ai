#ifndef _AI_H_
#define _AI_H_

struct Evaluation {
  double score;
  bool valid;

  Evaluation(double score): score(score), valid(true) {}
  Evaluation(double score, bool valid): score(score), valid(valid) {}

  static Evaluation invalid() {
    return Evaluation(0.0, false);
  }
};

#endif
