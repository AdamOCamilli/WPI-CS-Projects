(TeX-add-style-hook
 "Assignment_3"
 (lambda ()
   (TeX-add-to-alist 'LaTeX-provided-class-options
                     '(("article" "12pt")))
   (TeX-add-to-alist 'LaTeX-provided-package-options
                     '(("geometry" "left=1in" "right=1in" "top=1in" "bottom=1in")))
   (TeX-run-style-hooks
    "latex2e"
    "article"
    "art12"
    "geometry"
    "amsmath"
    "amssymb"
    "tcolorbox")))

