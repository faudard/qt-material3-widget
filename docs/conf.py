from __future__ import annotations

import os
from pathlib import Path

project = "qt-material3-widget"
author = "faudard"
release = "0.4.0"

extensions = [
    "myst_parser",
    "breathe",
    "sphinx.ext.autosectionlabel",
    "sphinx.ext.githubpages",
]

source_suffix = {
    ".rst": "restructuredtext",
    ".md": "markdown",
}

exclude_patterns = [
    "_build",
    "Thumbs.db",
    ".DS_Store",
]

myst_enable_extensions = [
    "colon_fence",
    "deflist",
]

autosectionlabel_prefix_document = True

breathe_projects = {
    "qtmaterial3": str((Path(__file__).resolve().parent.parent / "build" / "doxygen" / "xml").resolve()),
}
breathe_default_project = "qtmaterial3"
breathe_domain_by_extension = {
    "h": "cpp",
    "hpp": "cpp",
}

primary_domain = "cpp"
highlight_language = "cpp"

html_theme = "furo"
html_title = "qt-material3-widget"
html_baseurl = os.environ.get("DOCS_BASE_URL", "")

nitpicky = False
