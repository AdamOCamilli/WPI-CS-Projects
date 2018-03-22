(TeX-add-style-hook
 "Homework_1"
 (lambda ()
   (TeX-add-to-alist 'LaTeX-provided-class-options
                     '(("article" "10pt")))
   (TeX-add-to-alist 'LaTeX-provided-package-options
                     '(("geometry" "top=0.8in" "bottom=0.8in")))
   (TeX-run-style-hooks
    "latex2e"
    "article"
    "art10"
    "geometry"
    "amsmath"
    "amssymb"
    "graphicx"
    "tikz"
    "mathtools"
    "hyperref")
   (TeX-add-symbols
    '("verbatimfont" 1))
   (LaTeX-add-environments
    "answer"))
 :latex)

