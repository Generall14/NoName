%% Definicje:
%% MAINTITLE - główny tytuł
%% SUBTITLE - podtytuł
%% DESCTITLE - opis pod tutułem
%% WERSJA - pełny opis wersji
%% NUMER - numer dokumentu
%% HEADERNAME - nazwa produktu w nagłówku
%% AUTOR - ??

%% TPWARNING - ostrzeżenie na stronie tytułowej

%% Makra:
%% \maketitle - tworzy stronę tytułową
%% \maketableofcontents - tworzy spis treści
%% \pabox{arg1} - wyświetla tekst arg1 w ramce
%% \papbox{arg1}{arg2} - wyświetla tekst arg2 w ramce z obrazem arg1
%% \paabox{arg1} - wyświetla tekst arg1 w ramce z obrazem UWAGA!

%% Znaczniki:
%% \TODO - wstawia kolorowy znacznik z napisem <UZUPEŁNIĆ>
%% \CONF - wstawia kolorowy znacznik z napisem <POTWIERDZIĆ>

\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{Base}[2016/10/17 Standard LaTeX class]

\LoadClass{article}
\RequirePackage[T1]{fontenc}
\RequirePackage[polish]{babel}
\RequirePackage[utf8]{inputenc}
\selectlanguage{polish}
\RequirePackage{eso-pic}
\RequirePackage{graphicx}
\RequirePackage{datetime}
\RequirePackage{geometry}
\RequirePackage{parskip}
\RequirePackage{chngcntr}
\RequirePackage{caption}
\RequirePackage{fancyhdr}
\RequirePackage{lastpage}
\RequirePackage{hyperref, url}
\RequirePackage{tabularx}
\RequirePackage{footnote}
\RequirePackage[titletoc]{appendix}
\RequirePackage{amsmath}
\RequirePackage{amsthm,algorithm,algorithmic,yhmath,enumitem,lscape}
\RequirePackage{float}
\RequirePackage{epstopdf}
\RequirePackage{listings}
\RequirePackage{longtable}
\RequirePackage{nameref}

\DeclareOption{inprogress}{\def\TPINPROGRESS{Wersja robocza!}}
\ProcessOptions\relax

%% Znaczniki:
\def\TODO{{\color{blue}<UZUPEŁNIĆ>}}
\def\CONF{{\color{red}<POTWIERDZIĆ>}}

%% Wartości domyślne:
\def\HEADERNAME{ }
\def\TITLEPICTURE{classfiles/tlo.jpg}
\def\TRASHPICTURE{classfiles/trash}
\def\WARNINGPICTURE{classfiles/warning.png}
\def\DEFWIDTH{0.65\linewidth}

%format daty:
\setdefaultdate{\yyyymmdddate}
\renewcommand{\date}{\formatdate{\day}{\month}{\year}}
\renewcommand{\dateseparator}{.}

\bibliographystyle{plain}
\newcommand{\makebibliography}
{
	\bibliography{bibliografia} %Edycja wpisów w pliku bibliografia.bib
	\addcontentsline{toc}{section}{Literatura}
}

\if@twoside
	\geometry{left=35mm,top=35mm, right=25mm, bottom=20mm}			%marginesy strony
\else
	\geometry{left=30mm,top=35mm, right=30mm, bottom=20mm}			%marginesy strony
\fi

\renewcommand*\rmdefault{qhv}										%Czcionka

\counterwithin{figure}{section}										%ustawianie numerowania rysunków
\captionsetup[figure]{name=Rys.}									%Oznaczenie rysunków
\captionsetup[table]{name=Tab.}										%Oznaczenie tabel
\captionsetup{font=small,textfont=sf}								%czcionka opisu
\captionsetup{labelfont=bf}											%pogrubienie podpisu
\counterwithin{table}{section}										%ustawianie numerowania tabeli

\raggedbottom														%Wyłączenie justowania w pionie
\hypersetup{hidelinks}												%ukrywanie podświetlania hiperlinków

% \newcommand\asd{
\fancyhead{}														%Ustawienia nagłówków
\fancyfoot{}
\pagestyle{fancy}
\def\lewymar{\nouppercase{\sc\footnotesize\leftmark}}
\def\prawymar{\thepage\space/ \pageref{LastPage}}
\fancyhead[LO]{\lewymar}
\fancyhead[LE]{\prawymar}
\fancyhead[C]{\HEADERNAME}
\fancyhead[RO]{\prawymar}
\fancyhead[RE]{\lewymar}
\renewcommand{\headrulewidth}{0.2pt}
% }

\newenvironment{cminipage}{\begin{center}\begin{minipage}[h]{0.7\textwidth}}{\end{minipage}\end{center}}
\newcommand{\byte}[1]{\texttt{[#1]}}

\newcommand*{\papbox}[2]
{
	\framebox[\textwidth][c]
	{
		\raisebox{-.3\baselineskip}{\includegraphics[height=2\baselineskip]{#1}}
		
		\parbox{.9\textwidth}
		{
			\begin{center}
				\textbf{{#2}}
			\end{center}
		}
	}
}

\newcommand*{\pabox}[1]
{
	\framebox[\textwidth][c]
	{
		\parbox{.9\textwidth}
		{
			\begin{center}
				\textbf{{#1}}
			\end{center}
		}
	}
}

\newcommand*{\paabox}[1]
{
	\papbox{\WARNINGPICTURE}{#1}
}

\renewcommand\maketitle
{
\begin{titlepage}
	\thispagestyle{empty}
	
	%\AddToShipoutPicture*{\put(0,0){\includegraphics{\TITLEPICTURE}}}
	\vspace*{3cm}
	
	\vspace*{2cm}
	
	\begin{center}
		\ifdefined\MAINTITLE
			\Huge{\textbf{\MAINTITLE}}\\
			\vspace{10mm}
		\fi
		\ifdefined\SUBTITLE
			\huge{\SUBTITLE}\\
			\vspace{5mm}
		\fi
		\ifdefined\DESCTITLE
			\LARGE{\DESCTITLE}\\
			\vspace{15mm}
		\fi
		\ifdefined\TPINPROGRESS
			{\color{red}{\Huge{\textbf \TPINPROGRESS}}}\\
			\vspace{5mm}
		\fi
		\ifdefined\NUMER
			\Large{\texttt{\NUMER}}\\
			\vspace{5mm}
		\fi
		\ifdefined\WERSJA
			\Large{\texttt{\WERSJA}}\\
			\vspace{5mm}
		\fi
		\Large{\texttt{\date}}
		
		\ifdefined\AUTOR
			\vspace{5mm}
			\large{Autor: \AUTOR}\\
		\fi
	\end{center}
\end{titlepage}
}%

\newcommand\maketableofcontents
{
	\pagenumbering{roman}											%Numerowanie 
	\tableofcontents												%Spis treści
	\newpage
	\pagenumbering{arabic}
}
