from __future__ import annotations

import os
from pathlib import Path

DOCS_DIR = Path(__file__).resolve().parent
ROOT_DIR = DOCS_DIR.parent
DOXYGEN_XML_DIR = ROOT_DIR / "build" / "doxygen" / "xml"

project = "qt-material3-widget"
author = "faudard"
release = "0.4.0"

extensions = [
    "myst_parser",
    "breathe",
    "sphinx.ext.autosectionlabel",
    "sphinx.ext.githubpages",
    "sphinxcontrib.mermaid",
]

source_suffix = {
    ".rst": "restructuredtext",
    ".md": "markdown",
}

master_doc = "index"
exclude_patterns = [
    "_build",
    "Thumbs.db",
    ".DS_Store",
]

myst_enable_extensions = [
    "colon_fence",
    "deflist",
]

myst_heading_anchors = 3
myst_fence_as_directive = ["mermaid"]
mermaid_output_format = "raw"
autosectionlabel_prefix_document = True

breathe_projects = {
    "qtmaterial3": str(DOXYGEN_XML_DIR),
}
breathe_default_project = "qtmaterial3"
breathe_domain_by_extension = {
    "h": "cpp",
    "hpp": "cpp",
}
breathe_show_include = True

primary_domain = "cpp"
highlight_language = "cpp"
default_role = "any"

html_theme = "furo"
html_title = project
html_baseurl = os.environ.get("DOCS_BASE_URL", "")
html_static_path = []

nitpicky = False

# Optional local hint when Sphinx is run before Doxygen.
if not DOXYGEN_XML_DIR.exists():
    print(
        "warning: Doxygen XML directory not found at "
        f"{DOXYGEN_XML_DIR}. Run 'doxygen Doxyfile' before 'sphinx-build'."
    )
