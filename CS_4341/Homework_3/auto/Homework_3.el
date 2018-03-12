(TeX-add-style-hook
 "Homework_3"
 (lambda ()
   (TeX-add-to-alist 'LaTeX-provided-class-options
                     '(("article" "10pt")))
   (TeX-add-to-alist 'LaTeX-provided-package-options
                     '(("geometry" "left=1in" "right=1in" "top=1in" "bottom=1in") ("inputenc" "utf8")))
   (TeX-run-style-hooks
    "latex2e"
    "article"
    "art10"
    "geometry"
    "amssymb"
    "enumitem"
    "graphicx"
    "array"
    "inputenc"
    "mathtools"
    "empheq"
    "tikz"
    "hyperref")
   (LaTeX-add-environments
    "answer"))
 :latex)

