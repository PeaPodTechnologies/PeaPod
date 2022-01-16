# PeaPod Documentation

Documents are split into two types:
- Publishing Documents (i.e. for submission to DSFC) - Written in LaTeX
- Reference Documents (i.e. API docs, repo docs) - Written in Markdown

Each active document is placed in its own folder. Sections shared between documents are turned into "modules" and are placed in the `tex/` folder for import (i.e. `\input{../tex/Module.tex}`). Shared assets (i.e. graphics) are placed in the `assets/` folder for import. 

Archived/read-only document folders (i.e. DSFC Phase 1) are placed in the `archive/` folder.