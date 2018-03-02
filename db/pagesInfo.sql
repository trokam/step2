-- Database generated with pgModeler (PostgreSQL Database Modeler).
-- pgModeler  version: 0.8.1
-- PostgreSQL version: 9.4
-- Project Site: pgmodeler.com.br
-- Model Author: ---


-- Database creation must be done outside an multicommand file.
-- These commands were put in this file only for convenience.
-- -- object: new_database | type: DATABASE --
-- -- DROP DATABASE IF EXISTS new_database;
-- CREATE DATABASE new_database
-- ;
-- -- ddl-end --
-- 

-- object: public.sequence | type: TABLE --
-- DROP TABLE IF EXISTS public.sequence CASCADE;
CREATE TABLE public.sequence(
	index serial NOT NULL,
	value varchar(200) NOT NULL,
	CONSTRAINT text_pkey PRIMARY KEY (index),
	CONSTRAINT value_unique UNIQUE (value)

);
-- ddl-end --
ALTER TABLE public.sequence OWNER TO postgres;
-- ddl-end --

-- object: public.page | type: TABLE --
-- DROP TABLE IF EXISTS public.page CASCADE;
CREATE TABLE public.page(
	index serial NOT NULL,
	protocol varchar(10),
	index_domain integer,
	path varchar(1000),
	level integer NOT NULL,
	processing bool NOT NULL,
	crunched date NOT NULL,
	state smallint NOT NULL,
	CONSTRAINT page_pkey PRIMARY KEY (index)

);
-- ddl-end --
ALTER TABLE public.page OWNER TO postgres;
-- ddl-end --

-- object: public.page_seq | type: TABLE --
-- DROP TABLE IF EXISTS public.page_seq CASCADE;
CREATE TABLE public.page_seq(
	index_page integer,
	index_sequence integer,
	index_period integer,
	count integer NOT NULL,
	relevance_in_body smallint NOT NULL,
	relevance_in_url smallint NOT NULL,
	relevance_in_title smallint NOT NULL,
	relevance_total integer NOT NULL
);
-- ddl-end --
ALTER TABLE public.page_seq OWNER TO postgres;
-- ddl-end --

-- object: page_fk | type: CONSTRAINT --
-- ALTER TABLE public.page_seq DROP CONSTRAINT IF EXISTS page_fk CASCADE;
ALTER TABLE public.page_seq ADD CONSTRAINT page_fk FOREIGN KEY (index_page)
REFERENCES public.page (index) MATCH FULL
ON DELETE SET NULL ON UPDATE CASCADE;
-- ddl-end --

-- object: sequence_fk | type: CONSTRAINT --
-- ALTER TABLE public.page_seq DROP CONSTRAINT IF EXISTS sequence_fk CASCADE;
ALTER TABLE public.page_seq ADD CONSTRAINT sequence_fk FOREIGN KEY (index_sequence)
REFERENCES public.sequence (index) MATCH FULL
ON DELETE SET NULL ON UPDATE CASCADE;
-- ddl-end --

-- object: public.trait | type: TABLE --
-- DROP TABLE IF EXISTS public.trait CASCADE;
CREATE TABLE public.trait(
	index_page integer,
	title varchar(1000) NOT NULL,
	content_size integer NOT NULL,
	complexity float NOT NULL
);
-- ddl-end --
ALTER TABLE public.trait OWNER TO postgres;
-- ddl-end --

-- object: page_fk | type: CONSTRAINT --
-- ALTER TABLE public.trait DROP CONSTRAINT IF EXISTS page_fk CASCADE;
ALTER TABLE public.trait ADD CONSTRAINT page_fk FOREIGN KEY (index_page)
REFERENCES public.page (index) MATCH FULL
ON DELETE SET NULL ON UPDATE CASCADE;
-- ddl-end --

-- object: trait_uq | type: CONSTRAINT --
-- ALTER TABLE public.trait DROP CONSTRAINT IF EXISTS trait_uq CASCADE;
ALTER TABLE public.trait ADD CONSTRAINT trait_uq UNIQUE (index_page);
-- ddl-end --

-- object: public.period | type: TABLE --
-- DROP TABLE IF EXISTS public.period CASCADE;
CREATE TABLE public.period(
	index serial NOT NULL,
	start date NOT NULL,
	CONSTRAINT period_pkey PRIMARY KEY (index)

);
-- ddl-end --
ALTER TABLE public.period OWNER TO postgres;
-- ddl-end --

-- object: public.seq_occ | type: TABLE --
-- DROP TABLE IF EXISTS public.seq_occ CASCADE;
CREATE TABLE public.seq_occ(
	index_sequence integer,
	index_period integer,
	count integer NOT NULL
);
-- ddl-end --
ALTER TABLE public.seq_occ OWNER TO postgres;
-- ddl-end --

-- object: sequence_fk | type: CONSTRAINT --
-- ALTER TABLE public.seq_occ DROP CONSTRAINT IF EXISTS sequence_fk CASCADE;
ALTER TABLE public.seq_occ ADD CONSTRAINT sequence_fk FOREIGN KEY (index_sequence)
REFERENCES public.sequence (index) MATCH FULL
ON DELETE SET NULL ON UPDATE CASCADE;
-- ddl-end --

-- object: period_fk | type: CONSTRAINT --
-- ALTER TABLE public.seq_occ DROP CONSTRAINT IF EXISTS period_fk CASCADE;
ALTER TABLE public.seq_occ ADD CONSTRAINT period_fk FOREIGN KEY (index_period)
REFERENCES public.period (index) MATCH FULL
ON DELETE SET NULL ON UPDATE CASCADE;
-- ddl-end --

-- object: period_fk | type: CONSTRAINT --
-- ALTER TABLE public.page_seq DROP CONSTRAINT IF EXISTS period_fk CASCADE;
ALTER TABLE public.page_seq ADD CONSTRAINT period_fk FOREIGN KEY (index_period)
REFERENCES public.period (index) MATCH FULL
ON DELETE SET NULL ON UPDATE CASCADE;
-- ddl-end --

-- object: unique_page_text | type: CONSTRAINT --
-- ALTER TABLE public.page_seq DROP CONSTRAINT IF EXISTS unique_page_text CASCADE;
ALTER TABLE public.page_seq ADD CONSTRAINT unique_page_text UNIQUE (index_page,index_sequence,index_period);
-- ddl-end --

-- object: unique_seq_occ | type: CONSTRAINT --
-- ALTER TABLE public.seq_occ DROP CONSTRAINT IF EXISTS unique_seq_occ CASCADE;
ALTER TABLE public.seq_occ ADD CONSTRAINT unique_seq_occ UNIQUE (index_sequence,index_period);
-- ddl-end --

-- object: public.domain | type: TABLE --
-- DROP TABLE IF EXISTS public.domain CASCADE;
CREATE TABLE public.domain(
	index serial NOT NULL,
	value varchar(250) NOT NULL,
	type smallint NOT NULL,
	CONSTRAINT domain_pkey PRIMARY KEY (index),
	CONSTRAINT domain_unique UNIQUE (value)

);
-- ddl-end --
ALTER TABLE public.domain OWNER TO postgres;
-- ddl-end --

-- object: domain_fk | type: CONSTRAINT --
-- ALTER TABLE public.page DROP CONSTRAINT IF EXISTS domain_fk CASCADE;
ALTER TABLE public.page ADD CONSTRAINT domain_fk FOREIGN KEY (index_domain)
REFERENCES public.domain (index) MATCH FULL
ON DELETE SET NULL ON UPDATE CASCADE;
-- ddl-end --

-- object: page_unique | type: CONSTRAINT --
-- ALTER TABLE public.page DROP CONSTRAINT IF EXISTS page_unique CASCADE;
ALTER TABLE public.page ADD CONSTRAINT page_unique UNIQUE (protocol,index_domain,path);
-- ddl-end --


