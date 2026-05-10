# Building the CMPE-220 PDF from `cpu_design_report.tex`

The course report is a single pdfLaTeX source: **`cpu_design_report.tex`**. It uses `geometry`, `setspace`, `hyperref`, `listings`, `booktabs`, and TikZ (Overleaf compiles with **pdfLaTeX**).

## Local build (macOS / Linux)

```bash
cd "CPU Design/docs"
pdflatex -interaction=nonstopmode cpu_design_report.tex
pdflatex -interaction=nonstopmode cpu_design_report.tex
```

Run **twice** so the table of contents and cross-references settle. Output: `cpu_design_report.pdf`.

## Packages

Install a TeX distribution if needed (MacTeX, TeX Live, MiKTeX). No shell-escape is required for the current TikZ figure.

## Git hygiene

Commit LaTeX sources, not build artifacts (`*.aux`, `*.log`, `*.out`, `*.pdf`) unless your instructor requires the PDF in-repo—add those patterns to `.gitignore` if you compile locally.
